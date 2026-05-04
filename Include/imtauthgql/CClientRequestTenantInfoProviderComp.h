// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantInfoProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CClientRequestTenantInfoProviderComp:
			virtual public imtauth::ITenantInfoProvider,
			public imtclientgql::CClientRequestManagerCompBase
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestTenantInfoProviderComp);
		I_REGISTER_INTERFACE(imtauth::ITenantInfoProvider);
		I_ASSIGN(m_tenantFactoryCompPtr, "TenantFactory", "Tenant factory", true, "TenantFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::ITenantInfoProvider)
	virtual const imtbase::ICollectionInfo& GetTenantList() const override;
	virtual imtauth::ITenantInfoSharedPtr GetTenant(const QByteArray& tenantId, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	
protected:
	I_FACT(imtauth::ITenantInfo, m_tenantFactoryCompPtr);
};


} // namespace imtauthgql


