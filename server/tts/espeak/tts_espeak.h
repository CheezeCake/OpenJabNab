#ifndef _TTSESPEAK_H_
#define _TTSESPEAK_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
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
};

#endif

