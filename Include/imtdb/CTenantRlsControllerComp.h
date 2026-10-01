// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/ITenantRlsController.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


/**
	Component enforcing tenant Row-Level Security on shared Postgres tables.
	The isolation policies compare the tenant ID column against the configured
	session variable; sessions without a bound tenant match no rows (fail-closed).
*/
class CTenantRlsControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ITenantRlsController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantRlsControllerComp)
		I_REGISTER_INTERFACE(imtdb::ITenantRlsController);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN_MULTI_0(m_tableNamesAttrPtr, "TableNames", "Names of the shared tables protected by tenant Row-Level Security", true);
		I_ASSIGN(m_tableSchemaAttrPtr, "TableSchema", "Schema containing the shared tables", false, "public");
		I_ASSIGN(m_tenantIdColumnAttrPtr, "TenantIdColumn", "Name of the column containing the tenant ID", false, "TenantId");
		I_ASSIGN(m_sessionVariableNameAttrPtr, "SessionVariableName", "Name of the session variable carrying the tenant ID of the current database session", false, "app.tenant_id");
		I_ASSIGN(m_applyOnStartupAttrPtr, "ApplyOnStartup", "Apply the Row-Level Security policies when the component is created", false, false);
	I_END_COMPONENT;

	// reimplemented (imtdb::ITenantRlsController)
	virtual bool ApplyRowLevelSecurity() override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool IsPostgresDriver() const;

	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_MULTIATTR(QByteArray, m_tableNamesAttrPtr);
	I_ATTR(QByteArray, m_tableSchemaAttrPtr);
	I_ATTR(QByteArray, m_tenantIdColumnAttrPtr);
	I_ATTR(QByteArray, m_sessionVariableNameAttrPtr);
	I_ATTR(bool, m_applyOnStartupAttrPtr);
};


} // namespace imtdb
