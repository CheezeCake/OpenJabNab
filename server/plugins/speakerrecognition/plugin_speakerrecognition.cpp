#include <vector>
#include "plugin_speakerrecognition.h"
#include "settings.h"
#include "bunny.h"
#include "context.h"
#include "ttsmanager.h"
#include "messagepacket.h"
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

bool PluginSpeakerRecognition::OnClick(Bunny* b, PluginInterface::ClickType)
{
	if(b->IsIdle())
	{
		if(Context::getAvailability())
		{
			QString recordRoot = GlobalSettings::GetString("Config/RealHttpRoot") + "/plugins/record/";
			QString filename = b->GetGlobalSetting("LastRecord").toString();
			QString filepath = recordRoot + filename;
			std::vector<std::string> path;
			path.push_back(filepath.toStdString());
			QString result(speakerReco::identify(path).c_str());

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
