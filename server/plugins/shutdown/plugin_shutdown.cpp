#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "plugin_shutdown.h"
#include "settings.h"
#include "bunny.h"
#include "context.h"
#include "ttsmanager.h"
#include "messagepacket.h"
#include "pocketSphinx.h"

Q_EXPORT_PLUGIN2(plugin_shutdown, PluginShutdown)

const std::string PluginShutdown::shutdownPath = "../resources/scripts/shutdownServer.sh";
const std::string PluginShutdown::shutdownName = "shutdownServer";

std::string PluginShutdown::oldPlugin;
std::string PluginShutdown::oldSpecs;

PluginShutdown::PluginShutdown():PluginInterface("shutdown", "shutdown plugin")
{}

PluginShutdown::~PluginShutdown() {}

void PluginShutdown::shutdown()
{
	pid_t pid = fork();
	int status = 1;

	if(pid == 0)
	{
		execl(shutdownPath.c_str(), shutdownName.c_str(), NULL);
		std::cerr << "Error exec " << shutdownPath << '\n';
		exit(1);
	}
	// kind of useless
	else if(pid > 0)
		waitpid(pid, &status, 0);
	else
		std::cerr << "Fork failed for Shutdown" << std::endl;
}

bool PluginShutdown::OnClick(Bunny* b, PluginInterface::ClickType)
{
	if(b->IsIdle())
	{
		if(Context::getAvailability())
		{
			QByteArray file = TTSManager::CreateNewSound("Dis arrêter pour éteindre le système ou annuler pour ne rien faire", "julie");

			if(!file.isNull())
			{
				QByteArray message = "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
				Context::setActivePlugin("shutdown");
				return true;
			}

		}
		else if(Context::getActivePlugin() == "shutdown")
		{
			QString filename = b->GetGlobalSetting("LastRecord", "").toString();
			QString result = PocketSphinx::recognize(filename);
			if(result == "arrêter")
			{
				QByteArray file = TTSManager::CreateNewSound("Le système va maintenant s'arrêter", "julie");

				if(!file.isNull())
				{
					QByteArray message = "MU "+file+"\nPL 3\nMW\n";
					b->SendPacket(MessagePacket(message));
					shutdown();
					return true;
				}
			}
			else if(result == "oublier")
			{
				QByteArray file = TTSManager::CreateNewSound("L'application va maintenant s'arrêter", "julie");

				if(!file.isNull())
				{
					QByteArray message = "MU "+file+"\nPL 3\nMW\n";
					b->SendPacket(MessagePacket(message));
					Context::reset();
					return true;
				}
			}
			else
			{
				QByteArray file = TTSManager::CreateNewSound("Action annulée", "julie");

				if(!file.isNull())
				{
					QByteArray message = "MU "+file+"\nPL 3\nMW\n";
					b->SendPacket(MessagePacket(message));
					Context::setActivePlugin(oldPlugin);
					Context::setPluginSpecs(oldSpecs);
					return true;
				}
				
			}
		}
		// WHEN A PLUGIN IS RUNNING
		else
		{
			oldPlugin = Context::getActivePlugin();
			oldSpecs = Context::getPluginSpecs();

			QByteArray file = TTSManager::CreateNewSound("Dis arrêter pour éteindre le système, oublier pour arrêter l'application lancée ou annuler pour ne rien faire", "julie");

			if(!file.isNull())
			{
				QByteArray message = "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
				Context::setActivePlugin("shutdown");
				return true;
			}
		}
	}

	return false;
}
