#include "plugin_speakerrecognition.h"

Q_EXPORT_PLUGIN2(plugin_speakerrecognition, PluginSpeakerRecognition)

PluginSpeakerRecognition::PluginSpeakerRecognition():PluginInterface("speakerrecognition", "speakerrecognition plugin")
{
}

PluginSpeakerRecognition::~PluginSpeakerRecognition() {}

void PluginSpeakerRecognition::OnBunnyConnect(Bunny *)
{
}

void PluginSpeakerRecognition::OnBunnyDisconnect(Bunny *)
{
}

void PluginSpeakerRecognition::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
{
}
