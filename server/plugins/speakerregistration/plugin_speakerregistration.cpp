#include "plugin_speakerregistration.h"
#include "speakerReco.h"
#include "context.h"
#include "settings.h"
#include "bunny.h"
#include "ttsmanager.h"
#include "messagepacket.h"
#include "pocketSphinx.h"

Q_EXPORT_PLUGIN2(plugin_speakerregistration, PluginSpeakerRegistration)

QString PluginSpeakerRegistration::speakerName = "";
std::vector<std::string> PluginSpeakerRegistration::files;

PluginSpeakerRegistration::PluginSpeakerRegistration():PluginInterface("speakerregistration", "speakerregistration plugin")
{}

PluginSpeakerRegistration::~PluginSpeakerRegistration() {}

//void PluginSpeakerRegistration::OnBunnyConnect(Bunny *)
//{}

//void PluginSpeakerRegistration::OnBunnyDisconnect(Bunny *)
//{}

//void PluginSpeakerRegistration::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
//{}

bool PluginSpeakerRegistration::OnClick(Bunny* b, PluginInterface::ClickType)
{
	if(b->IsIdle())
	{
		if(Context::getAvailability())
		{
			QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
			QByteArray file = TTSManager::CreateNewSound("Comment dois-je t'appeler ?" , "julie");

			if(!file.isNull())
			{
				QByteArray message = "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
				Context::setActivePlugin("speakerregistration");
				Context::setPluginSpecs("name");
				return true;
			}
			return false;
		}
		else if(Context::getActivePlugin() == "speakerregistration")
		{
			if(Context::getPluginSpecs() == "name")
			{
				QString filename = b->GetGlobalSetting("LastRecord", "").toString();
				speakerName = PocketSphinx::recognize(filename);
				// SEE IF speakerName is persistent

				QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
				QByteArray file = TTSManager::CreateNewSound("D'accord" + speakerName + ", peux-tu me parler un peu plusse ?", "julie");

				if(!file.isNull())
				{
					QByteArray message = "MU "+file+"\nPL 3\nMW\n";
					b->SendPacket(MessagePacket(message));
					Context::setPluginSpecs("voice");
					return true;
				}
				return false;
			}
			else if(Context::getPluginSpecs() == "voice")
			{
				const int nbAudioFiles = 4;
				int size = files.size();
				if(size >= nbAudioFiles)
				{
					speakerReco::registerModel(speakerName.toStdString(), files);
					
					QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
					QByteArray file = TTSManager::CreateNewSound("Merci " + speakerName + ", tu es maintenant enregistré" , "julie");
					if(!file.isNull())
					{
						QByteArray message = "MU "+file+"\nPL 3\nMW\n";
						b->SendPacket(MessagePacket(message));
						Context::reset();
						files.clear();
						return true;
					}
					return false;
				}

				QString recordRoot = GlobalSettings::GetString("Config/RealHttpRoot", "") + "plugins/record/";
				QString filename = b->GetGlobalSetting("LastRecord", "").toString();
				std::string filepath = recordRoot.toStdString() + filename.toStdString();
				files.push_back(filepath);

				if (size + 1 <= nbAudioFiles)
				{
					QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
					QByteArray file = TTSManager::CreateNewSound(QString("Merci %1, j'ai encore besoin que tu me parles %2 fois").arg(speakerName, QString::number(nbAudioFiles - size - 1)), "julie");

					if(!file.isNull())
					{
						QByteArray message = "MU "+file+"\nPL 3\nMW\n";
						b->SendPacket(MessagePacket(message));
						return true;
					}
				}
				return false;
			}
		}
	}
	return false;
}
