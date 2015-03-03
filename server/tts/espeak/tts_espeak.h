#ifndef _TTSESPEAK_H_
#define _TTSESPEAK_H_

#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include <sndfile.hh>
#include <espeak/speak_lib.h>
#include "ttsinterface.h"

class TTSESpeak : public TTSInterface
{
	Q_OBJECT
	Q_INTERFACES(TTSInterface)

public:
	TTSESpeak();
	virtual ~TTSESpeak();
	QByteArray CreateNewSound(QString, QString, bool);

private:
	static SndfileHandle* fileHandle;
	int sampleRate;

	static int callback(short*, int, espeak_EVENT*);
};

#endif

