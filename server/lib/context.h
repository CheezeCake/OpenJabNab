#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <iostream>
#include <ctime>

class Context
{
	private:
		static const double timeout; // Timeout in seconds
		static std::time_t waitingSince; // Time registered when a plugin is made active
		static bool isAvailable; // True if no plugin are running
		static std::string activePlugin; // Name of the active plugin
		static std::string pluginSpecifics; // Information for the running plugin (state for a several step scenario for ex)

	public:
		static void init(); // calls reset (sementics)
		static void reset(); //resets all values to initial
		static void resetTime(); //resets time only (used when a plugin uses context more than once)
		static void update(); //checks if timeout has passed, resets if so

		static bool getAvailability();

		static void setActivePlugin(std::string);
		static std::string getActivePlugin();
		static void setPluginSpecs(std::string);
		static std::string getPluginSpecs();
};

#endif
