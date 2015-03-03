#include <QDateTime>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_espeak.h"
#include "log.h"

Q_EXPORT_PLUGIN2(tts_espeak, TTSESpeak)

TTSESpeak::TTSESpeak():TTSInterface("espeak", "ESpeak")
{
}

TTSESpeak::~TTSESpeak()
{}

QByteArray TTSESpeak::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	// TO DO : MANAGE forceOverwrite + QString to string ? + close eSpeak

	int samplerate = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, DATA_DIR, 0);

	if(samplerate == -1)
	{
		std::cerr << "Error initialize\n";
		LogError("eSpeak dit not initialize properly")
		return QByteArray();
	}

	espeak_SetVoiceByName(voice);

	fileHandle = SndfileHandle("out.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16,
			1, sr);

	if (!fileHandle)
		return 3;

	espeak_SetSynthCallback(callback);

	int ret = espeak_Synth(text, sizeof(text), 0,
			POS_CHARACTER, 0, espeakCHARS_UTF8, NULL, NULL);

	return 0;
}

int TTSESpeak::callback(short* wav int numsamples, espeak_EVENT* events)
{
	// debug
	//std::cout << "callback called\n";

	if(wav)
	{
		// debug
		//std::cout << "writing!\n";
		fileHandle.write(wav, numsamples);
	}

	return 0;
}
