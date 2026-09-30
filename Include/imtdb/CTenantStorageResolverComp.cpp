// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageResolverComp.h>


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

	m_registry.SetSharedFallbackEnabled(*m_allowSharedFallbackAttrPtr);
	m_registry.SetSharedSchemaName(*m_sharedSchemaNameAttrPtr);
	m_registry.SetSchemaNamePrefix(*m_schemaNamePrefixAttrPtr);

	if (*m_allowSharedFallbackAttrPtr){
		SendWarningMessage(
					0,
					QStringLiteral("Shared storage fallback is enabled: unregistered tenants resolve to the shared schema '%1'").arg(QString(*m_sharedSchemaNameAttrPtr)),
					"CTenantStorageResolverComp");
	}
}


} // namespace imtdb
