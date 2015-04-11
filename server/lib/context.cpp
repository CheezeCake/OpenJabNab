#include <iostream>
#include <ctime>
#include "context.h"

void Context::init()
{
	reset();
}

void Context::reset()
{
	Context::waitingSince = 0;
	Context::isAvailable = true;
	Context::activePlugin = "";
	Context::pluginSpecifics = "";
}

void Context::resetTime()
{
	Context::waitingSince = time(NULL);
}

void Context::update()
{
	if(Context::isAvailable)
		return;

	if(difftime(time(NULL), Context::waitingSince) > Context::timeout)
		reset();
}

bool Context::getAvailability()
{
	return Context::isAvailable;
}

void Context::setActivePlugin(std::string newPlugin)
{
	Context::activePlugin = newPlugin;
	Context::isAvailable = false;
	setPluginSpecs("");
}

std::string Context::getActivePlugin()
{
	return Context::activePlugin;
}

void Context::setPluginSpecs(std::string newSpecs)
{
	Context::pluginSpecifics = newSpecs;
}

std::string Context::getPluginSpecs()
{
	return Context::pluginSpecifics;
}
