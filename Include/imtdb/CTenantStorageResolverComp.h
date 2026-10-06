// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ITenantStorageResolver.h>
#include <imtdb/CTenantStorageRegistry.h>


namespace imtdb
{


/**
	Component resolving the physical storage location of a tenant.
	Resolution is fail-closed by default: tenants without an explicit storage
	assignment are rejected unless \c AllowSharedFallback is enabled.
	All registration and failed resolution events are audit-logged
	(EU CRA, Annex I Part I 2(l)).
*/
class CTenantStorageResolverComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ITenantStorageResolver
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantStorageResolverComp);
		I_REGISTER_INTERFACE(imtdb::ITenantStorageResolver);
		I_ASSIGN(m_allowSharedFallbackAttrPtr, "AllowSharedFallback", "Allow resolving unregistered tenants to the shared schema (disables fail-closed mode)", false, false);
		I_ASSIGN(m_sharedSchemaNameAttrPtr, "SharedSchemaName", "Name of the shared database schema", false, "public");
		I_ASSIGN(m_schemaNamePrefixAttrPtr, "SchemaNamePrefix", "Prefix used to derive dedicated tenant schema names", false, "tenant_");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Optional engine of the shared catalog; if set, the persisted storage assignments are loaded from the TenantStorage table on startup", false, "DatabaseEngine");
		I_ASSIGN(m_registryTableSchemaAttrPtr, "RegistryTableSchema", "Schema containing the TenantStorage registry table", false, "");
	I_END_COMPONENT;

	// reimplemented (imtdb::ITenantStorageResolver)
	virtual bool ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const override;
	virtual bool RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info) override;
	virtual bool UnregisterTenantStorage(const QByteArray& tenantId) override;
	virtual bool IsTenantStorageRegistered(const QByteArray& tenantId) const override;
	virtual QByteArrayList GetRegisteredTenantIds() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void LoadPersistedAssignments();

	I_ATTR(bool, m_allowSharedFallbackAttrPtr);
	I_ATTR(QByteArray, m_sharedSchemaNameAttrPtr);
	I_ATTR(QByteArray, m_schemaNamePrefixAttrPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_ATTR(QByteArray, m_registryTableSchemaAttrPtr);

	CTenantStorageRegistry m_registry;
};


} // namespace imtdb
