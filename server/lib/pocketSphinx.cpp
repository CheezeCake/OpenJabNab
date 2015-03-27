#include <unistd.h>
#include <sys/wait.h>
#include "pocketSphinx.h"
#include "settings.h"
#include "log.h"

ps_decoder_t* PocketSphinx::ps = 0;

PocketSphinx& PocketSphinx::getInstance()
{
	static PocketSphinx p;
	return p;
}

void PocketSphinx::init()
{
	LogInfo("-- Initializing Pocketsphinx --");

	cmd_ln_t* cfg;

	cfg = cmd_ln_init(NULL, ps_args(), TRUE,
			"-hmm", "model/hmm/lium_french_f0",
			"-lm", "model/lm/fr-phone.lm.dmp",
			"-dict", "model/lm/frenchWords62K.dic",
			NULL);

	if (cfg)
	{

		ps = ps_init(cfg);
		if (!ps)
			LogError("Error initializing PocketSphinx (could not init ps_decoder_t structure)");
	}
	else
	{
		LogError("Error initializing PocketSphinx (could not init cmld_ln_t structure)");
	}

	LogInfo("-- Pocketsphinx initialized --");
}

PocketSphinx::~PocketSphinx()
{
	if (ps)
		ps_free(ps);
}

PocketSphinx::PocketSphinx()
{
	if (!ps)
		LogWarning("PocketSphinx has not been init()ialized");
}

QString PocketSphinx::recognize(const QString& filename)
{
	FILE* filep;
	const char *hyp;
	const char *uttid;
	int rv;
	int32 score;
	QString recordRoot = GlobalSettings::GetString("RealHttpRoot") + "plugins/record/";
	QString converted = recordRoot + "ps_" + filename;
	QString file = recordRoot + filename;

	// convert to PCM signed 16kHz
	pid_t pid = fork();
	if (pid == 0)
	{
		execl("sox", "sox", file.toStdString().c_str(), "-b16",
				converted.toStdString().c_str(), "rate", "16k", NULL);

		LogError("Convertion failed");
		exit(1);
	}

	waitpid(pid, 0, 0);

	filep = fopen(converted.toStdString().c_str(), "rb");
	if (filep)
	{
		rv = ps_decode_raw(ps, filep, NULL, -1);
		if (rv >= 0)
		{
			hyp = ps_get_hyp(ps, &score, &uttid);
			if (hyp)
			{
				fclose(filep);
				return QString(hyp);
			}
			else
			{
				LogError("Error decoding file");
			}
		}
		else
		{
			LogError("Error decoding file");
		}
	}
	else
	{
		LogError("Error openning file");
	}

	return QString();
}
