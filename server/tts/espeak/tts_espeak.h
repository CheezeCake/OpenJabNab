#ifndef _TTSESPEAK_H_
#define _TTSESPEAK_H_

#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include <sndfile.hh>
#include <speak_lib.h>
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
	SndfileHandle fileHandle;

	int callback(short*, numsamples, espeak_EVENT*);
};

#endif

