// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageAutoProvisioningResolverComp.h>


// Qt includes
#include <QtConcurrent/QtConcurrent>


namespace imtdb
{


// public methods

CTenantStorageAutoProvisioningResolverComp::CTenantStorageAutoProvisioningResolverComp()
{
	m_provisioningThreadPool.setMaxThreadCount(1);
}


// reimplemented (imtdb::ITenantStorageResolver)

bool CTenantStorageAutoProvisioningResolverComp::ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const
{
	if (!tenantId.isEmpty() && !m_storageResolverCompPtr->IsTenantStorageRegistered(tenantId)){
		std::lock_guard lock(m_provisioningMutex);

		if (!m_storageResolverCompPtr->IsTenantStorageRegistered(tenantId)){
			SendInfoMessage(0, QStringLiteral("Provisioning the storage of tenant '%1' on its first access").arg(QString(tenantId)), "CTenantStorageAutoProvisioningResolverComp");

			ITenantStorageProvisioner* provisionerPtr = m_storageProvisionerCompPtr.GetPtr();
			QFuture<bool> provisioning = QtConcurrent::run(&m_provisioningThreadPool, [provisionerPtr, tenantId](){
				return provisionerPtr->ProvisionTenantStorage(tenantId);
			});

			if (!provisioning.result()){
				SendErrorMessage(0, QStringLiteral("Storage of tenant '%1' could not be provisioned, access denied").arg(QString(tenantId)), "CTenantStorageAutoProvisioningResolverComp");

				return false;
			}
		}
	}

	return m_storageResolverCompPtr->ResolveTenantStorage(tenantId, result);
}


bool CTenantStorageAutoProvisioningResolverComp::RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info)
{
	return m_storageResolverCompPtr->RegisterTenantStorage(tenantId, info);
}


bool CTenantStorageAutoProvisioningResolverComp::UnregisterTenantStorage(const QByteArray& tenantId)
{
	return m_storageResolverCompPtr->UnregisterTenantStorage(tenantId);
}


bool CTenantStorageAutoProvisioningResolverComp::IsTenantStorageRegistered(const QByteArray& tenantId) const
{
	return m_storageResolverCompPtr->IsTenantStorageRegistered(tenantId);
}


QByteArrayList CTenantStorageAutoProvisioningResolverComp::GetRegisteredTenantIds() const
{
	return m_storageResolverCompPtr->GetRegisteredTenantIds();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTenantStorageAutoProvisioningResolverComp::OnComponentDestroyed()
{
	m_provisioningThreadPool.waitForDone();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


