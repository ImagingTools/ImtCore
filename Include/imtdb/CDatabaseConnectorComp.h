// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseConnector.h>


namespace imtdb
{


// PostgresDatabaseConnector
class CDatabaseConnectorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::IDatabaseConnector
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseConnectorComp)
		I_REGISTER_INTERFACE(imtdb::IDatabaseConnector);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine", true, "DatabaseEngine");
		I_ASSIGN(m_databaseLoginSettingsCompPtr, "RemoteDatabaseLoginSettings", "Remote database login settings", true, "RemoteDatabaseLoginSettings");
		I_ASSIGN(m_serverNameAttrPtr, "ServerName", "Server name", false, "ServerName");
		I_ASSIGN(m_schemaNameAttrPtr, "SchemaName", "Schema name", false, "public");
		I_ASSIGN_MULTI_0(m_tableNamesAttrPtr, "TableNames", "Table names which we want to import (if count = 0, then we import all the tables)", false);
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::IDatabaseConnector)
	virtual bool ConnectToDatabase(const IDatabaseLoginSettings& loginSettings) const override;
	virtual bool DisconnectFromDatabase(const QString& connectionName) const override;
	
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_ATTR(QString, m_serverNameAttrPtr);
	I_ATTR(QString, m_schemaNameAttrPtr);
	I_MULTIATTR(QString, m_tableNamesAttrPtr);
};


} // namespace imtdb

