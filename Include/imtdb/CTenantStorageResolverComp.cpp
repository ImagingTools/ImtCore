// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageResolverComp.h>


// ImtCore includes
#include <imtdb/CTenantStorageDbStore.h>


namespace imtdb
{


// reimplemented (imtdb::ITenantStorageResolver)

bool CTenantStorageResolverComp::ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const
{
	if (tenantId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant storage resolution rejected: empty tenant ID"), "CTenantStorageResolverComp");

		return false;
	}

	if (!m_registry.ResolveTenantStorage(tenantId, result)){
		SendWarningMessage(
					0,
					QStringLiteral("Tenant storage resolution failed (fail-closed): no storage assignment for tenant '%1'").arg(QString(tenantId)),
					"CTenantStorageResolverComp");

		return false;
	}

	return true;
}


bool CTenantStorageResolverComp::RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info)
{
	if (!m_registry.RegisterTenantStorage(tenantId, info)){
		SendErrorMessage(
					0,
					QStringLiteral("Rejected invalid tenant storage assignment for tenant '%1'").arg(QString(tenantId)),
					"CTenantStorageResolverComp");

		return false;
	}

	SendInfoMessage(
				0,
				QStringLiteral("Registered storage assignment for tenant '%1' (kind %2, schema '%3', connection '%4')")
					.arg(QString(tenantId))
					.arg(int(info.storageKind))
					.arg(QString(info.schemaName))
					.arg(QString(info.connectionRef)),
				"CTenantStorageResolverComp");

	return true;
}


bool CTenantStorageResolverComp::UnregisterTenantStorage(const QByteArray& tenantId)
{
	if (!m_registry.UnregisterTenantStorage(tenantId)){
		return false;
	}

	SendInfoMessage(
				0,
				QStringLiteral("Removed storage assignment for tenant '%1'").arg(QString(tenantId)),
				"CTenantStorageResolverComp");

	return true;
}


bool CTenantStorageResolverComp::IsTenantStorageRegistered(const QByteArray& tenantId) const
{
	return m_registry.IsTenantStorageRegistered(tenantId);
}


QByteArrayList CTenantStorageResolverComp::GetRegisteredTenantIds() const
{
	return m_registry.GetRegisteredTenantIds();
}


// reimplemented (icomp::CComponentBase)

void CTenantStorageResolverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const bool allowSharedFallback = m_allowSharedFallbackAttrPtr.IsValid() ? *m_allowSharedFallbackAttrPtr : false;
	const QByteArray sharedSchemaName = m_sharedSchemaNameAttrPtr.IsValid() ? *m_sharedSchemaNameAttrPtr : QByteArrayLiteral("public");

	m_registry.SetSharedFallbackEnabled(allowSharedFallback);
	m_registry.SetSharedSchemaName(sharedSchemaName);
	m_registry.SetSchemaNamePrefix(m_schemaNamePrefixAttrPtr.IsValid() ? *m_schemaNamePrefixAttrPtr : QByteArrayLiteral("tenant_"));

	if (allowSharedFallback){
		SendWarningMessage(
					0,
					QStringLiteral("Shared storage fallback is enabled: unregistered tenants resolve to the shared schema '%1'").arg(QString(sharedSchemaName)),
					"CTenantStorageResolverComp");
	}

	if (m_databaseEngineCompPtr.IsValid()){
		LoadPersistedAssignments();
	}
}


// private methods

void CTenantStorageResolverComp::LoadPersistedAssignments()
{
	CTenantStorageDbStore store(*m_databaseEngineCompPtr, m_registryTableSchemaAttrPtr.IsValid() ? *m_registryTableSchemaAttrPtr : QByteArray());

	CTenantStorageDbStore::Assignments assignments;
	if (!store.EnsureRegistryTable() || !store.LoadAssignments(assignments)){
		SendErrorMessage(0, QStringLiteral("Persisted tenant storage assignments could not be loaded, all tenant storage resolutions will be rejected"), "CTenantStorageResolverComp");

		return;
	}

	int loadedCount = 0;
	for (const CTenantStorageDbStore::Assignment& assignment: assignments){
		if (m_registry.RegisterTenantStorage(assignment.first, assignment.second)){
			++loadedCount;
		}
		else{
			SendWarningMessage(0, QStringLiteral("Skipped invalid persisted storage assignment for tenant '%1'").arg(QString(assignment.first)), "CTenantStorageResolverComp");
		}
	}

	SendInfoMessage(0, QStringLiteral("Loaded %1 persisted tenant storage assignment(s)").arg(loadedCount), "CTenantStorageResolverComp");
}


} // namespace imtdb
