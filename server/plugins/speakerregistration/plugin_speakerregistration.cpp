#include "plugin_speakerregistration.h"
#include "speakerReco.h"
#include "context.h"
#include "settings.h"
#include "bunny.h"
#include "pocketSphinx.h"

Q_EXPORT_PLUGIN2(plugin_speakerregistration, PluginSpeakerRegistration)

PluginSpeakerRegistration::PluginSpeakerRegistration():PluginInterface("speakerregistration", "speakerregistration plugin")
{}

PluginSpeakerRegistration::~PluginSpeakerRegistration() {}

//void PluginSpeakerRegistration::OnBunnyConnect(Bunny *)
//{}

//void PluginSpeakerRegistration::OnBunnyDisconnect(Bunny *)
//{}

//void PluginSpeakerRegistration::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
//{}

bool PluginSpeakerRegistration::OnClick(Bunny*, PluginInterface::ClickType)
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
				QString filename = b->GetGlobalSetting("LastRecord");
				QString name = pocketSphinx::recognize(filename);
				speakerName = name.toString();
				// SEE IF speakerName is persistent

				QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
				QByteArray file = TTSManager::CreateNewSound("D'accord" + speakerName + ", peux-tu me parler un peu plus ?" , "julie");

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
				QString recordRoot = GlobalSettings::GestString("Config/RealHttpRoot") + "/plugins/record/";
				QString filename = b->GetGlobalSetting("LastRecord");
				std::string file = recordRoot.toString() + filename.toString();
				std::vector<std::string> path;
				path.push_back(file);
				std::string result = speakerReco::registerModel(speakerName, path);

				QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
				QByteArray file = TTSManager::CreateNewSound("Merci" + speakerName + ", tu es maintenant enregistré" , "julie");

				if(!file.isNull())
				{
					QByteArray message = "MU "+file+"\nPL 3\nMW\n";
					b->SendPacket(MessagePacket(message));
					Context::reset();
					return true;
				}
				return false;
			}
		}
	}
	return false;
}
