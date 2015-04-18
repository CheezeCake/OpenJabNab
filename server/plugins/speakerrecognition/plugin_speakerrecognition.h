#ifndef _PLUGINSPEAKERRECOGNITION_H_
#define _PLUGINSPEAKERRECOGNITION_H_

#include "plugininterface.h"
	
class PluginSpeakerRecognition : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSpeakerRecognition();
	virtual ~PluginSpeakerRecognition();

	//void OnBunnyConnect(Bunny *);
	//void OnBunnyDisconnect(Bunny *);
	//void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
	bool OnClick(Bunny*, PluginInterface::ClickType);
};

#endif
