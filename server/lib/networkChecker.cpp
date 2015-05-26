#include <QNetworkInterface>
#include "networkChecker.h"

bool NetworkChecker::networkAvailable()
{
	QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

	for (int i = 0; i < ifaces.count(); i++) {
		QNetworkInterface iface = ifaces.at(i);

		if (iface.humanReadableName() == "eth0" && 
				iface.flags().testFlag(QNetworkInterface::IsUp))
			return (iface.addressEntries().count() > 0);
	}

	return false;
}
