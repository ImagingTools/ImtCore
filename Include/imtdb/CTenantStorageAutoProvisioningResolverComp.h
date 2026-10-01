// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThreadPool>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/ITenantStorageProvisioner.h>
#include <imtdb/ITenantStorageResolver.h>

// std includes
#include <mutex>


namespace imtdb
{


/**
	Tenant storage resolver provisioning the storage of a tenant on its first access.
	For servers that do not create tenants themselves (tenants are created by the authorization
	server), so no tenant creation event is available to provision on. The tenant ID comes from
	the request context, i.e. from a token validated by the authorization server.
	Provisioning runs on a dedicated thread: its DDL must not join the transaction of the request
	that triggered it, whose rollback would leave a registered tenant without a schema.
	Concurrent first accesses are serialized.
*/
class CTenantStorageAutoProvisioningResolverComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ITenantStorageResolver
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantStorageAutoProvisioningResolverComp);
		I_REGISTER_INTERFACE(imtdb::ITenantStorageResolver);
		I_ASSIGN(m_storageResolverCompPtr, "StorageResolver", "Resolver holding the storage assignments (also used by the provisioner)", true, "TenantStorageResolver");
		I_ASSIGN(m_storageProvisionerCompPtr, "StorageProvisioner", "Provisioner creating the storage of a tenant on its first access", true, "TenantStorageProvisioner");
	I_END_COMPONENT;

	CTenantStorageAutoProvisioningResolverComp();

	// reimplemented (imtdb::ITenantStorageResolver)
	virtual bool ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const override;
	virtual bool RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info) override;
	virtual bool UnregisterTenantStorage(const QByteArray& tenantId) override;
	virtual bool IsTenantStorageRegistered(const QByteArray& tenantId) const override;
	virtual QByteArrayList GetRegisteredTenantIds() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	I_REF(imtdb::ITenantStorageResolver, m_storageResolverCompPtr);
	I_REF(imtdb::ITenantStorageProvisioner, m_storageProvisionerCompPtr);

	mutable std::mutex m_provisioningMutex;
	mutable QThreadPool m_provisioningThreadPool;
};


} // namespace imtdb


