#include <QDateTime>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_espeak.h"
#include "log.h"

Q_EXPORT_PLUGIN2(tts_espeak, TTSESpeak)

SNDFILE* TTSESpeak::fileHandle = 0;

TTSESpeak::TTSESpeak() : TTSInterface("espeak", "ESpeak")
{
	sampleRate = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, "/usr/share", 0);

	if (sampleRate == -1)
		LogError("eSpeak dit not initialize properly");
}

TTSESpeak::~TTSESpeak()
{
	if (sampleRate != -1)
		espeak_Terminate();
}

QByteArray TTSESpeak::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	if (sampleRate == -1)
	{
		LogError("eSpeak not initialized");
		return QByteArray();
	}

	if (!voiceList.contains(voice))
		voice = "fr";

	// Check (and create if needed) output folder
	QDir outputFolder = ttsFolder;
	if (!outputFolder.exists(voice))
		outputFolder.mkdir(voice);

	if (!outputFolder.cd(voice))
	{
		LogError(QString("Cant create TTS Folder : %1").arg(ttsFolder.absoluteFilePath(voice)));
		return QByteArray();
	}

	// Compute fileName
	QString fileName = QCryptographicHash::hash(text.toAscii(), QCryptographicHash::Md5).toHex().append(".wav");
	QString filePath = outputFolder.absoluteFilePath(fileName);

	if (!forceOverwrite && QFile::exists(filePath))
		return ttsHTTPUrl.arg(voice, fileName).toAscii();


	// TODO : QString to string ?

	espeak_SetVoiceByName(voice.toStdString().c_str());

	SF_INFO sfInfo;
	memset(&sfInfo, 0, sizeof(SF_INFO));
	sfInfo.channels = 1;
	sfInfo.samplerate = sampleRate;
	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	fileHandle = sf_open(filePath.toStdString().c_str(), SFM_WRITE, &sfInfo);

	if (!fileHandle)
	{
		LogError("Cannot open sound file for writing");
		return QByteArray();
	}

	espeak_SetSynthCallback(callback);

	if (espeak_Synth(text.toStdString().c_str(), text.size(), 0, POS_CHARACTER, text.size(), espeakCHARS_UTF8, NULL, NULL) != EE_OK)
	{
		LogError("eSpeak synthesis failed");
		sf_close(fileHandle);
		fileHandle = 0;
		return QByteArray();
	}

	sf_write_sync(fileHandle);
	sf_close(fileHandle);
	fileHandle = 0;
	return ttsHTTPUrl.arg(voice, fileName).toAscii();
}

int TTSESpeak::callback(short* wav, int numsamples, espeak_EVENT* events)
{
	(void)events;
	if (wav && numsamples > 0)
		sf_write_short(fileHandle, wav, numsamples);

	return 0;
}
