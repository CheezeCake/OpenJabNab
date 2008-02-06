#include <QDir>
#include <QPluginLoader>
#include <QString>
#include <QLibrary>
#include <QCoreApplication>

#include "pluginmanager.h"
#include "log.h"

PluginManager::PluginManager()
{
	// Load all plugins
	QDir pluginsDir = QCoreApplication::applicationDirPath();
	pluginsDir.cd("plugins");

	Log::Info(QString("Finding plugins in : %1").arg(pluginsDir.path()));
	
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QString file = pluginsDir.absoluteFilePath(fileName);
		if (!QLibrary::isLibrary(file))
			continue;

		QString status = QString(" - %1 : ").arg(fileName);
		
		QPluginLoader loader(file);
		QObject * p = loader.instance();
		PluginInterface * plugin = qobject_cast<PluginInterface *>(p);
		if (plugin)
		{
			listOfPlugins.append(plugin);
			status.append(" OK");
		}
		else
		{
			status.append("Failed, ").append(loader.errorString()); 
		}
		Log::Info(status);
	}
}

PluginManager::~PluginManager()
{
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		delete plugin;
	}
}

void PluginManager::HttpRequestBefore(HTTPRequest const& request)
{
	// Call RequestBefore for all plugins
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		plugin->HttpRequestBefore(request);
	}
}

bool PluginManager::HttpRequestHandle(HTTPRequest & request)
{
	// Call GetAnswer for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->HttpRequestHandle(request))
			return true;
	}
	return false;
}

void PluginManager::HttpRequestAfter(HTTPRequest const& request)
{
	// Call RequestAfter for all plugins
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		plugin->HttpRequestAfter(request);
	}
}
	
void PluginManager::XmppBunnyMessage(QByteArray const& data)
{
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		plugin->XmppBunnyMessage(data);
	}
}

void PluginManager::XmppVioletMessage(QByteArray const& data)
{
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		plugin->XmppVioletMessage(data);
	}
}
void PluginManager::XmppVioletPacketMessage(Packet * p)
{
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		plugin->XmppVioletPacketMessage(p);
	}
}

bool PluginManager::OnClick(PluginInterface::ClickType type)
{
	// Call OnClick for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->OnClick(type))
			return true;
	}
	return false;
}

bool PluginManager::OnEarsMove(int left, int right)
{
	// Call OnClick for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->OnEarsMove(left, right))
			return true;
	}
	return false;
}