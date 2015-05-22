#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkChecker.h"

const QString NetworkChecker::Url("http://google.com");

bool NetworkChecker::networkAvailable()
{
	QNetworkAccessManager accessManager;
	QNetworkReply* reply = accessManager.get(QNetworkRequest(Url));

	return (reply && reply->error() == QNetworkReply::NoError);
}
