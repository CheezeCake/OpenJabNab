#ifndef _PLUGINCERI_H_
#define _PLUGINCERI_H_

#include "plugininterface.h"
	
class PluginCERI : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginCERI();
	virtual ~PluginCERI();

	bool OnClick(Bunny* b, PluginInterface::ClickType);

private:
	void rabbitTalk(std::string text, Bunny* b);
	void answer(std::string name, Bunny* b);
};

#endif
