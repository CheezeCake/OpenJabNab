#ifndef _PLUGINSPEAKERREGISTRATION_H_
#define _PLUGINSPEAKERREGISTRATION_H_

#include "plugininterface.h"
	
class PluginSpeakerRegistration : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSpeakerRegistration();
	virtual ~PluginSpeakerRegistration();

	//void OnBunnyConnect(Bunny *);
	//void OnBunnyDisconnect(Bunny *);
	//void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
	bool OnClick(Bunny*, PluginInterface::ClickType);

private:
	static std::string speakerName;
};

#endif
