#ifndef _PLUGINJOKE_H_
#define _PLUGINJOKE_H_

#include "plugininterface.h"
	
class PluginJoke : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginJoke();
	virtual ~PluginJoke();

	bool OnClick(Bunny* b, PluginInterface::ClickType);
};

#endif
