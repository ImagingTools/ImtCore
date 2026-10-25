// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantManager);
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection", true, "TenantCollection");
		I_ASSIGN(m_tenantFactoryCompPtr, "TenantFactory", "Tenant info factory", true, "TenantInfo");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantManager)
	virtual QByteArrayList GetTenantIds() const override;
	virtual ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId) const override;
	virtual QByteArrayList GetOwnedOrCreatedTenantIds(const QByteArray& userId) const override;
	virtual QByteArrayList GetChildTenantIds(const QByteArray& tenantId) const override;
	virtual QByteArrayList FindTenantIdsByName(const QString& tenantName) const override;
	virtual QByteArrayList GetTenantPermissions(const QByteArray& tenantId) const override;
	virtual QByteArray CreateTenant(
				const QString& tenantName,
				const QString& description = QString(),
				const QByteArray& ownerId = QByteArray(),
				const QByteArray& creatorId = QByteArray(),
				const QByteArray& parentTenantId = QByteArray()) override;
	virtual bool RemoveTenant(const QByteArray& tenantId) override;
	virtual bool UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId = QByteArray(), bool updateOwner = false) override;
	virtual bool SetTenantActive(const QByteArray& tenantId, bool isActive) override;
	virtual bool SetTenantPermissions(const QByteArray& tenantId, const QByteArrayList& permissions) override;
	virtual bool SetTenantHierarchy(const QByteArray& tenantId, const QByteArray& parentTenantId) override;
	virtual QByteArray GetSystemTenantId() const override;
	virtual bool EnsureSystemTenant() override;

private:
	bool SetTenantPermissionsTree(const QByteArray& tenantId, const QByteArrayList& permissions, QSet<QByteArray>& visitedTenantIds);

	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_FACT(imtauth::ITenantInfo, m_tenantFactoryCompPtr);
};


} // namespace imtauth
