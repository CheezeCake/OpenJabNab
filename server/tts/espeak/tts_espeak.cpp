#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_espeak.h"
#include "log.h"

Q_EXPORT_PLUGIN2(tts_espeak, TTSESpeak)

TTSESpeak::TTSESpeak():TTSInterface("espeak", "ESpeak")
{}

TTSESpeak::~TTSESpeak()
{}

QByteArray TTSESpeak::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{}
