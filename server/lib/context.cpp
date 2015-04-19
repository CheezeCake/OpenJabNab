#include <iostream>
#include <ctime>
#include "context.h"
#include "log.h"

const double Context::timeout = 60 * 3;
std::time_t Context::waitingSince = 0;
bool Context::isAvailable = true;
std::string Context::activePlugin = "";
std::string Context::pluginSpecifics = "";

void Context::init()
{
	reset();
}

void Context::reset()
{
	waitingSince = 0;
	isAvailable = true;
	activePlugin = "";
	pluginSpecifics = "";
}

void Context::resetTime()
{
	waitingSince = time(NULL);
}

void Context::update()
{
	if(isAvailable)
		return;

	if(difftime(time(NULL), waitingSince) > timeout)
		reset();
}

bool Context::getAvailability()
{
	return isAvailable;
}

void Context::setActivePlugin(std::string newPlugin)
{
	activePlugin = newPlugin;
	isAvailable = false;
	setPluginSpecs("");
}

std::string Context::getActivePlugin()
{
	return activePlugin;
}

void Context::setPluginSpecs(std::string newSpecs)
{
	pluginSpecifics = newSpecs;
	resetTime();
}

std::string Context::getPluginSpecs()
{
	return pluginSpecifics;
}
