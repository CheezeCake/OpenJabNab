#include "plugin_speakerregistration.h"

Q_EXPORT_PLUGIN2(plugin_speakerregistration, PluginSpeakerRegistration)

PluginSpeakerRegistration::PluginSpeakerRegistration():PluginInterface("speakerregistration", "speakerregistration plugin")
{
}

PluginSpeakerRegistration::~PluginSpeakerRegistration() {}

void PluginSpeakerRegistration::OnBunnyConnect(Bunny *)
{
}

void PluginSpeakerRegistration::OnBunnyDisconnect(Bunny *)
{
}

void PluginSpeakerRegistration::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
{
}
