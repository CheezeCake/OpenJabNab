#include "plugin_ceri.h"
#include "bunny.h"
#include "context.h"
#include "ttsmanager.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_ceri, PluginCERI)

PluginCERI::PluginCERI():PluginInterface("ceri", "ceri plugin") {}

PluginCERI::~PluginCERI() {}

bool PluginCERI::OnClick(Bunny* b, PluginInterface::ClickType)
{
	if(! b->IsIdle())
		return false;

	if(! Context::getAvailability())
		return false;
	
	rabbitTalk("Bonjour, nous allons vous présenter le série", b);
	rabbitTalk("Monsieur Costa, qu'est-ce que le série ?", b);
	answer("CERI_Sigle", b);

	rabbitTalk("D'où vient le série ?", b);
	answer("CERI_Activite", b);

	rabbitTalk("Combien y a-t-il d'étudiants au série ?", b);
	answer("CERI_NB_Etudiants", b);

	rabbitTalk("Quelles formations sont proposées", b);
	answer("CERI_Formations", b);

	rabbitTalk("Quel niveau faut-il pour entrer au série ?", b);
	answer("CERI_Niveaux", b);

	rabbitTalk("Comment s'en sortent les étudiants ?", b);
	answer("CERI_Resultats", b);

	rabbitTalk("Les étudiants font-ils de la robotique ?", b);
	answer("CERI_Robot_V2(Avec_classement)", b);

	rabbitTalk("Merci beaucoup monsieur Costa, nous sommes maintenant plus famillier avec le série", b);

	return true;
}

void PluginCERI::rabbitTalk(std::string text, Bunny* b)
{
	QString voice = b->GetPluginSetting(GetName(), "voice", "tts").toString();
	QByteArray file = TTSManager::CreateNewSound(text.c_str(), "julie");

	if(!file.isNull())
	{
		QByteArray message = "MU "+file+"\nPL 3\nMW\n";
		b->SendPacket(MessagePacket(message));
	}
}

void PluginCERI::answer(std::string name, Bunny* b)
{
	/* QByteArray file = QString("../resources/ceri/%1.wav").arg(name.c_str()).toAscii(); */
	QByteArray file = QString("broadcast/ceri/%1.wav").arg(name.c_str()).toAscii();
	if(!file.isNull())
	{
		QByteArray message = "MU "+file+"\nPL 3\nMW\n";
		b->SendPacket(MessagePacket(message));
	}
}
