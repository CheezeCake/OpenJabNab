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
	if (sqlite3_open("agenda.db", &dbHandle) != SQLITE_OK)
	{
		LogError(QString("Error opening agenda db file").arg(sqlite3_errmsg(dbHandle)));
		dbHandle = 0;
	}
	else
	{
		LogInfo("agenda.db successfully opened");
		char* err = 0;
		sqlite3_exec(dbHandle,
				"create table if not exists events (id int primary key, date text, event text);",
				0, 0, &err);
		if (err)
		{
			LogError(QString("Error creating table events : %1").arg(err));
			sqlite3_free(err);
		}
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
		LogInfo(QString("agenda str = '%1'").arg(str));

		int code;
		sqlite3_stmt *stmt;

		QRegExp add("ajouter (.+) le ([^ ]+ [^ ]+)( a (.+) heure (.+)?)?");
		QRegExp list(".+ [a-z]+");
		QRegExp remove("supprimer ([0-9]+)");

		if (add.exactMatch(str))
		{
			const char sql[] = "insert into events (date, event) values(?, ?)";
			code = sqlite3_prepare_v2(dbHandle, sql, -1, &stmt, 0);
			if (code != SQLITE_OK) {
				LogError(QString(sqlite3_errstr(code)));
				return true;
			}

			QString cap = add.cap(3) + " " + add.cap(1); // [ a ..h..] event
			sqlite3_bind_text(stmt, 2, cap.toStdString().c_str(), -1, SQLITE_TRANSIENT);

			QString cap1 = add.cap(2);
			sqlite3_bind_text(stmt, 1, cap1.toStdString().c_str(), -1, SQLITE_TRANSIENT);

			LogInfo(QString("%1, %2").arg(cap, cap1));

			file = TTSManager::CreateNewSound((sqlite3_step(stmt) != SQLITE_ERROR)
					? QString("evenement cree avec l'identifiant %1").arg(sqlite3_last_insert_rowid(dbHandle))
					: "Erreur lors de l'insertion de l'evenement dans l'agenda",
					"julie");

			sqlite3_finalize(stmt);
		}
		else if (list.exactMatch(str))
		{
			const char sql[] = "select event from events where date = ?";
			code = sqlite3_prepare_v2(dbHandle, sql, -1, &stmt, 0);
			if (code != SQLITE_OK) {
				LogError(QString(sqlite3_errstr(code)));
				return true;
			}

			LogInfo(QString("str.std.c_str = '%1'").arg(str.toStdString().c_str()));

			sqlite3_bind_text(stmt, 1, str.toStdString().c_str(), -1, SQLITE_TRANSIENT);

			QString eventList;
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				QString event(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
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
			const char sql[] = "delete from events where id = ?";
			code = sqlite3_prepare_v2(dbHandle, sql, -1, &stmt, 0);
			if (code != SQLITE_OK) {
				LogError(QString(sqlite3_errstr(code)));
				return true;
			}

			QString cap = remove.cap(1);
			sqlite3_bind_text(stmt, 1, cap.toStdString().c_str(), cap.length(), SQLITE_TRANSIENT);

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
