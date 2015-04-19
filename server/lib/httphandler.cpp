#include <QByteArray>
#include <QMap>
#include <memory>
#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httphandler.h"
#include "httprequest.h"
#include "log.h"
#include "netdump.h"
#include "openjabnab.h"
#include "settings.h"
#include "pocketSphinx.h"
#include "context.h"

HttpHandler::HttpHandler(QTcpSocket * s, bool api, bool violetapi):pluginManager(PluginManager::Instance())
{
	incomingHttpSocket = s;
	httpApi = api;
	httpVioletApi = violetapi;
	bytesToReceive = 0;
	connect(s, SIGNAL(readyRead()), this, SLOT(ReceiveData()));
}

HttpHandler::~HttpHandler() {}

void HttpHandler::ReceiveData()
{
	receivedData += incomingHttpSocket->readAll();
	if(bytesToReceive == 0 && (receivedData.size() >= 4))
		bytesToReceive = *(int *)receivedData.left(4).constData();

	if(bytesToReceive != 0 && (receivedData.size() == bytesToReceive))
		HandleBunnyHTTPRequest();
}

void HttpHandler::HandleBunnyHTTPRequest()
{
	HTTPRequest request(receivedData);
	QString uri = request.GetURI();
	if (uri.startsWith("/ojn_api/"))
	{
		NetworkDump::Log("Api Call", request.GetRawURI());
		if(httpApi)
		{
			std::auto_ptr<ApiManager::ApiAnswer> apianswer(ApiManager::Instance().ProcessApiCall(uri.mid(9), request));
			QByteArray answer = apianswer->GetData();
			incomingHttpSocket->write(answer);
			NetworkDump::Log("Api Answer", answer);
		}
		else
			incomingHttpSocket->write("Api is disabled");
	}
	else if (uri.startsWith("/ojn/FR/api"))
	{
		NetworkDump::Log("Violet Api Call", request.GetRawURI());
		if(httpVioletApi)
		{
			std::auto_ptr<ApiManager::ApiAnswer> apianswer(ApiManager::Instance().ProcessApiCall(uri, request));
			QByteArray answer = apianswer->GetData();
			incomingHttpSocket->write(answer);
			NetworkDump::Log("Violet Api Answer", answer);
		}
		else
			incomingHttpSocket->write("Violet Api is disabled");
	}
	else
	{
		NetworkDump::Log("HTTP Request", request.GetRawURI());
		pluginManager.HttpRequestBefore(request);
		if (!pluginManager.HttpRequestHandle(request))
		{
			LogError(QString("Unable to handle HTTP Request : ") + request.toString());
			request.reply = "404 Not Found";
		}
		pluginManager.HttpRequestAfter(request);
		incomingHttpSocket->write(request.reply);
		if(!uri.contains("itmode.jsp") && !uri.contains(".mp3") && !uri.contains(".chor") && !uri.contains("bc.jsp") && request.reply.size() < 256) // Don't dump too big answers
			NetworkDump::Log("HTTP Answer", request.reply);

		Disconnect(); // disconnect so the bunny doesn't send the record again
		if (uri.startsWith("/vl/record.jsp") && PocketSphinx::enabled()) // audio record
		{
			QString sn = request.GetArg("sn");
			Bunny* bunny = BunnyManager::GetBunny(sn.toAscii());
			QString record = bunny->GetGlobalSetting("LastRecord", "").toString();

			LogInfo(record);
			Context::update();

			if (Context::getAvailability()) {
				LogInfo("available");

				QString recognized = PocketSphinx::recognize(record);
				LogInfo("Recognized : " + recognized);


				QMap<QString, QString> plugins;
				plugins.insert("heure", "clock");
				plugins.insert("reconnaissance", "speakerrecognition");
				plugins.insert("enregistrement", "speakerregistration");

				QMap<QString, QString>::const_iterator it = plugins.find(recognized);
				if (it != plugins.end()) {
					PluginInterface* p = PluginManager::Instance().GetPluginByName(*it);
					LogInfo("starting plugin : " + *it);

					if (p)
						p->OnClick(bunny, PluginInterface::SingleClick);
					else
						LogError("PluginManager returned null for plugin : " + *it);
				}
			}
			else {
				QString activePlugin(Context::getActivePlugin().c_str());
				PluginInterface* p = PluginManager::Instance().GetPluginByName(activePlugin);
				LogInfo("starting plugin (waiting) : " + activePlugin);

				if (p)
					p->OnClick(bunny, PluginInterface::SingleClick);
				else
					LogError("PluginManager returned null for plugin : " + activePlugin);
			}
		}
		return;
	}
	Disconnect();
}

void HttpHandler::Disconnect()
{
	incomingHttpSocket->disconnectFromHost();
	// Delete incomingHttpSocket when it will be disconnected
	connect(incomingHttpSocket, SIGNAL(disconnected()), incomingHttpSocket, SLOT(deleteLater()));
	deleteLater();
}
