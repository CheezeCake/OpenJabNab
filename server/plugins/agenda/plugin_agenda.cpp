#include <QRegExp>
#include <string>
#include <sqlite3.h>
#include <iostream>
#include "plugin_agenda.h"
#include "bunny.h"
#include "pocketSphinx.h"
#include "log.h"
#include "ttsmanager.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_agenda, PluginAgenda)

PluginAgenda::PluginAgenda():PluginInterface("agenda", "agenda plugin")
{
	if (sqlite3_open("file:agenda.db", &dbHandle) != SQLITE_OK)
	{
		LogError(QString("Error opening agenda db file").arg(sqlite3_errmsg(dbHandle)));
		dbHandle = 0;
	}
	else
	{
		LogInfo("agenda.db successfully opened");
		sqlite3_exec(dbHandle,
				"create table if not exists events (id integer primary key autoincrement, date text, event text)",
				0, 0, 0);
	}
}

PluginAgenda::~PluginAgenda()
{
	if (dbHandle)
	{
		if (sqlite3_close(dbHandle) != SQLITE_OK)
			LogError("Error closing agenda db file");
	}
}

//void PluginAgenda::OnBunnyConnect(Bunny *)
//{
//}
//
//void PluginAgenda::OnBunnyDisconnect(Bunny *)
//{
//}
//
//void PluginAgenda::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
//{
//}

bool PluginAgenda::OnClick(Bunny* b, PluginInterface::ClickType)
{
	if(b->IsIdle())
	{
		QByteArray file;

		QString str = PocketSphinx::getLastRecognized();
		str.remove(QRegExp("^agenda "));
		LogInfo(QString("agenda str = %1").arg(str));

		int code;
		sqlite3_stmt *stmt;

		QRegExp add("ajouter (.+) le (.+)( a ([0-9]+) heure ([0-9]+)?)?");
		QRegExp list("[0-9]+ .+");
		QRegExp remove("supprimer ([0-9]+)");

		if (add.exactMatch(str))
		{
			std::string sql("inset into events (date, event) values(?, ?)");
			code = sqlite3_prepare_v2(dbHandle, sql.c_str(), sql.size(), &stmt, 0);
			if (code != SQLITE_OK) {
				LogError(QString(sqlite3_errstr(code)));
				return true;
			}

			QString cap = add.cap(3) + " " + add.cap(1); // [ a ..h..] event
			sqlite3_bind_text(stmt, 2, cap.toStdString().c_str(), cap.length(), 0);

			cap = add.cap(2);
			sqlite3_bind_text(stmt, 1, cap.toStdString().c_str(), cap.length(), 0);

			file = TTSManager::CreateNewSound((sqlite3_step(stmt) == SQLITE_OK)
					? QString("evenement cree avec l'identifiant %1").arg(sqlite3_last_insert_rowid(dbHandle))
					: "Erreur lors de l'insertion de l'evenement dans l'agenda",
					"julie");

			sqlite3_finalize(stmt);
		}
		else if (list.exactMatch(str))
		{
			std::string sql("select event from events where date = ?");
			code = sqlite3_prepare_v2(dbHandle, sql.c_str(), sql.size(), &stmt, 0);
			if (code != SQLITE_OK) {
				LogError(QString(sqlite3_errstr(code)));
				return true;
			}

			sqlite3_bind_text(stmt, 1, str.toStdString().c_str(), str.length(), 0);

			QString eventList;
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				sqlite3_value* value = sqlite3_column_value(stmt, 2);
				QString event(reinterpret_cast<const char*>(sqlite3_value_text(value)));
				if (!event.isEmpty())
				{
					std::cout << event.toStdString() << '\n';
					eventList.append(event.append(","));
				}
			}

			file = TTSManager::CreateNewSound(eventList.isEmpty() ? "aucun evenement" : eventList, "julie");

			sqlite3_finalize(stmt);
		}
		else if (remove.exactMatch(str))
		{
			std::string sql("delete from events where id = ?");
			code = sqlite3_prepare_v2(dbHandle, sql.c_str(), sql.size(), &stmt, 0);
			if (code != SQLITE_OK) {
				LogError(QString(sqlite3_errstr(code)));
				return true;
			}

			QString cap = remove.cap(1);
			sqlite3_bind_text(stmt, 1, cap.toStdString().c_str(), cap.length(), 0);

			file = TTSManager::CreateNewSound((sqlite3_step(stmt) == SQLITE_OK)
					? QString("evenement %1 supprime").arg(cap)
					: QString("l'evenement %1 n'existe pas").arg(cap),
					"julie");

			sqlite3_finalize(stmt);
		}
		else
		{
			file = TTSManager::CreateNewSound("Commande agenda inconnue", "julie");
			LogInfo("No match in agenda commands for " + str);
			return true;
		}

		if(!file.isNull())
		{
			QByteArray message = "MU "+file+"\nPL 3\nMW\n";
			b->SendPacket(MessagePacket(message));
			return true;
		}
	}

	return false;
}
