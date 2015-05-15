#ifndef _PLUGINAGENDA_H_
#define _PLUGINAGENDA_H_

#include "plugininterface.h"
#include <sqlite3.h>

class PluginAgenda : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginAgenda();
	virtual ~PluginAgenda();

	//void OnBunnyConnect(Bunny *);
	//void OnBunnyDisconnect(Bunny *);
	//void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
	bool OnClick(Bunny* b, PluginInterface::ClickType);

private:
	sqlite3* dbHandle;
};

#endif
