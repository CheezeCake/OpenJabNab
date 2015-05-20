#ifndef _PLUGINSPEAKERREGISTRATION_H_
#define _PLUGINSPEAKERREGISTRATION_H_

#include "plugininterface.h"
#include <vector>
#include <string>

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
	static QString speakerName;
	static std::vector<std::string> files;
};

#endif
