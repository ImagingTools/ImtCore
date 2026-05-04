// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauthgql/CClientRequestManagerCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


class CClientRequestTenantManagerComp:
			public imtauthgql::CClientRequestManagerCompBase,
			virtual public imtauth::ITenantManager
{
public:
	typedef imtauthgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestTenantManagerComp)
		I_REGISTER_INTERFACE(imtauth::ITenantManager);
		I_ASSIGN(m_tenantFactoryCompPtr, "TenantFactory", "Tenant factory", true, "TenantFactory");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantManager)
	virtual QByteArrayList GetTenantIds() const override;
	virtual imtauth::ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId) const override;
	virtual QByteArray CreateTenant(const QString& tenantName, const QString& description = QString()) override;
	virtual bool RemoveTenant(const QByteArray& tenantId) override;
	virtual bool UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description) override;
	virtual bool SetTenantActive(const QByteArray& tenantId, bool isActive) override;

private:
	bool GetTenantDataSdl(const QByteArray& tenantId, sdl::imtauth::Tenants::CTenantData::V1_0& tenantData) const;
	bool SetTenantDataSdl(const QByteArray& tenantId, const sdl::imtauth::Tenants::CTenantData::V1_0& tenantData) const;

private:
	I_FACT(imtauth::ITenantInfo, m_tenantFactoryCompPtr);
};


} // namespace imtauthgql


