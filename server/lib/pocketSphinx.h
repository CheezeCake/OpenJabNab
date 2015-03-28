#ifndef _POCKETSPHINX_H_
#define _POCKETSPHINX_H_

#include <pocketsphinx.h>
#include <QString>
#include "global.h"

class OJN_EXPORT PocketSphinx
{
public:
	~PocketSphinx();
	static void init();
	static bool enabled();
	static QString recognize(const QString&);

private:
	PocketSphinx();
	PocketSphinx(PocketSphinx&);
	void operator=(const PocketSphinx&);

	static ps_decoder_t* ps;
};

#endif
