#include "plugin_joke.h"
#include "settings.h"
#include "bunny.h"
#include "context.h"
#include "ttsmanager.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_joke, PluginJoke)

PluginJoke::PluginJoke():PluginInterface("joke", "joke plugin")
{
}

PluginJoke::~PluginJoke() {}

bool PluginJoke::OnClick(Bunny* b, PluginInterface::ClickType)
{
	if(b->IsIdle() && Context::getAvailability())
	{
		QString jokeDirPath = "../resources/blagues";
		QDir jokeDir(jokeDirPath);
		if(jokeDir.exists())
		{
			QStringList list = jokeDir.entryList();
			if(list.count() > 0)
			{
				int rand = qrand() % list.count();
				QFile file(list.at(rand));
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QByteArray text;
					while (!file.atEnd())
					{
						text += file.readLine();
					}
					QString textMessage(text);
					QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
					QByteArray voiceFile = TTSManager::CreateNewSound(textMessage, "julie");

					if(!voiceFile.isNull())
					{
						QByteArray message = "MU " + voiceFile + "\nPL 3\nMW\n";
						b->SendPacket(MessagePacket(message));
						return true;
					}
				}
			}
		}
	}
	return false;
}
