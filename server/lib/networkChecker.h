#ifndef _NETWORKCHECKER_H_
#define _NETWORKCHECKER_H_

#include <QString>

class NetworkChecker
{
	public:
		static bool networkAvailable();
		static const QString Url;
};

#endif
