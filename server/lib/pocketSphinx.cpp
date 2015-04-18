#include <unistd.h>
#include <sys/wait.h>
#include "pocketSphinx.h"
#include "settings.h"
#include "log.h"

ps_decoder_t* PocketSphinx::ps = 0;

bool PocketSphinx::enabled()
{
	return (ps != 0);
}

void PocketSphinx::init()
{
	LogInfo("-- Initializing Pocketsphinx --");

	cmd_ln_t* cfg;

	cfg = cmd_ln_init(NULL, ps_args(), TRUE,
			"-hmm", "model/hmm/lium_french_f0",
			/* "-hmm", "model/hmm/lium_french_f2", */
			/* "-lm", "model/lm/fr-phone.lm.dmp", */
			/* "-lm", "model/lm/french3g62K.lm.dmp", */
			"-fsg", "model/lm/grammar.fsg",
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
	QString recordRoot = GlobalSettings::GetString("Config/RealHttpRoot") + "plugins/record/";
	QString converted = recordRoot + "ps_" + filename;
	QString file = recordRoot + filename;

	// convert to PCM signed 16kHz
	pid_t pid = fork();
	if (pid == 0)
	{
		execl("/usr/bin/sox", "/usr/bin/sox", file.toStdString().c_str(), "-b16",
				converted.toStdString().c_str(), "rate", "16k", NULL);

		LogError(QString("Convertion failed: %1 -> %2").arg(file, converted));
		exit(1);
	}

	int status = 0;
	waitpid(pid, &status, 0);

	if (status != 0)
		return QString();

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
				LogError(QString("Error decoding file: %1").arg(converted));
			}
		}
		else
		{
			LogError(QString("Error decoding file: %1").arg(converted));
		}
	}
	else
	{
		LogError(QString("Error openning file: %1").arg(converted));
	}

	return QString();
}
