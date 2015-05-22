#ifndef _PLUGINSHUTDOWN_H_
#define _PLUGINSHUTDOWN_H_

#include "plugininterface.h"
	
class PluginShutdown : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginShutdown();
	virtual ~PluginShutdown();

	bool OnClick(Bunny* b, PluginInterface::ClickType);

private:
	static std::string oldPlugin;
	static std::string oldSpecs;
	static const std::string shutdownPath;
	static const std::string shutdownName;

	void shutdown();
};

#endif
