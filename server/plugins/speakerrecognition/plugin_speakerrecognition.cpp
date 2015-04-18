#include <vector>
#include "plugin_speakerrecognition.h"
#include "settings.h"
#include "bunny.h"
#include "speakerReco.h"

Q_EXPORT_PLUGIN2(plugin_speakerrecognition, PluginSpeakerRecognition)

PluginSpeakerRecognition::PluginSpeakerRecognition():PluginInterface("speakerrecognition", "speakerrecognition plugin")
{}

PluginSpeakerRecognition::~PluginSpeakerRecognition() {}

//void PluginSpeakerRecognition::OnBunnyConnect(Bunny *)
//{}

//void PluginSpeakerRecognition::OnBunnyDisconnect(Bunny *)
//{}

//void PluginSpeakerRecognition::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
//{}

bool PluginSpeakerRecognition::OnClick(Bunny*, PluginInterface::ClickType)
{
	if(b->IsIdle())
	{
		if(Context::getAvailability())
		{
			QString recordRoot = GlobalSettings::GestString("Config/RealHttpRoot") + "/plugins/record/";
			QString filename = b->GetGlobalSetting("LastRecord");
			std::string file = recordRoot.toString() + filename.toString();
			std::vector<std::string> path;
			path.push_back(file);
			std::string result = speakerReco::identify(path);

			QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
			QByteArray file = TTSManager::CreateNewSound("Bonjour, " + result , "julie");

			if(!file.isNull())
			{
				QByteArray message = "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
				return true;
			}
		}
	}
	return false;
}
