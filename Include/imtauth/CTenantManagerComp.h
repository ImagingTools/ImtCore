// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


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
	virtual QByteArrayList GetTenantPermissions(const QByteArray& tenantId) const override;
	virtual QByteArray CreateTenant(const QString& tenantName, const QString& description = QString(), const QByteArray& ownerId = QByteArray()) override;
	virtual bool RemoveTenant(const QByteArray& tenantId) override;
	virtual bool UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId = QByteArray(), bool updateOwner = false) override;
	virtual bool SetTenantActive(const QByteArray& tenantId, bool isActive) override;
	virtual bool SetTenantHierarchy(const QByteArray& tenantId, const QByteArray& parentTenantId) override;
	virtual QByteArray GetSystemTenantId() const override;
	virtual bool EnsureSystemTenant() override;

private:
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_FACT(imtauth::ITenantInfo, m_tenantFactoryCompPtr);
};


} // namespace imtauth
