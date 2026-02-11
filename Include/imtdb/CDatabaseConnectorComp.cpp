// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDatabaseConnectorComp.h>


namespace imtdb
{


// protected methods

// reimplemented (imtdb::IDatabaseConnector)

bool CDatabaseConnectorComp::ConnectToDatabase(const IDatabaseLoginSettings& loginSettings) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'DatabaseEngine' was not set", "CDatabaseConnectorComp");
		return false;
	}
	
	QString host = loginSettings.GetHost();
	QString name = loginSettings.GetDatabaseName();
	QString userName = loginSettings.GetUserName();
	QString password = loginSettings.GetPassword();
	
	QString importQuery = QString(R"(IMPORT FOREIGN SCHEMA %0 FROM SERVER "%1" INTO public;)");
	
	int tableCount = m_tableNamesAttrPtr.GetCount();
	if (tableCount == 0){
		importQuery = importQuery.arg("", *m_serverNameAttrPtr);
	}
	else{
		QString value = QString("\"%0\" LIMIT TO (").arg(*m_schemaNameAttrPtr);
		for (int i = 0; i < m_tableNamesAttrPtr.GetCount(); i++){
			QString tableName = m_tableNamesAttrPtr[i];
			
			if (i > 0){
				value += ",";
			}
			
			value += "\"" + tableName + "\"";
		}
		
		value += ")";
		
		importQuery = importQuery.arg(value, *m_serverNameAttrPtr);
	}
	
	QString query =
		QString(R"(
			CREATE EXTENSION IF NOT EXISTS postgres_fdw;
			CREATE OR REPLACE FUNCTION setup_foreign_table_%0(
				server_nam TEXT,
				db_host TEXT,
				db_name TEXT,
				db_user TEXT,
				db_password TEXT,
				schema_name TEXT DEFAULT 'public'
			) RETURNS VOID AS $$
			DECLARE
				server_count INT;
				mapping_count INT;
			BEGIN
				SELECT COUNT(*) INTO server_count FROM pg_foreign_server WHERE srvname = server_nam;
				IF server_count = 0 THEN
					EXECUTE format(
						'CREATE SERVER %I FOREIGN DATA WRAPPER postgres_fdw OPTIONS (host %L, dbname %L)',
						server_nam, db_host, db_name
					);
				END IF;
			
				SELECT COUNT(*) INTO mapping_count FROM pg_user_mappings WHERE srvname = server_nam AND umuser = SESSION_USER::regrole;
				IF mapping_count = 0 THEN
					EXECUTE format(
						'CREATE USER MAPPING FOR CURRENT_USER SERVER %I OPTIONS (user %L, password %L)',
						server_nam, db_user, db_password
					);
				END IF;
			
				%1
			END;
			$$ LANGUAGE plpgsql;
		)").arg(*m_serverNameAttrPtr, importQuery);
	
	query +=
		QString(R"(
			SELECT setup_foreign_table_%0(
				'%0',
				'%1',
				'%2',
				'%3',
				'%4',
				'%5'
			);
		)").arg(*m_serverNameAttrPtr, host, name, userName, password, *m_schemaNameAttrPtr);
	
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		qDebug() << sqlError.text();
		SendErrorMessage(0, sqlError.text(), "CDatabaseConnectorComp");
		return false;
	}
	
	return true;
}


bool CDatabaseConnectorComp::DisconnectFromDatabase(const QString& connectionName) const
{
	QString query = QString(R"(DROP SERVER IF EXISTS "%0" CASCADE;)").arg(connectionName);
	
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		qDebug() << sqlError.text();
		SendErrorMessage(0, sqlError.text(), "CDatabaseConnectorComp");
		return false;
	}
	
	return true;
}


// reimplemented (icomp::CComponentBase)

void CDatabaseConnectorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	
	DisconnectFromDatabase(*m_serverNameAttrPtr);
	
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'DatabaseLoginSettings' was not set", "CDatabaseConnectorComp");
	}
	else{
		ConnectToDatabase(*m_databaseLoginSettingsCompPtr);
	}
}


void CDatabaseConnectorComp::OnComponentDestroyed()
{
	DisconnectFromDatabase(*m_serverNameAttrPtr);
	
	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


