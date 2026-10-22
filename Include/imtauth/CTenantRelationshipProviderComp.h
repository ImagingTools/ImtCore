// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantRelationshipProvider.h>
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantManager.h>


namespace imtauth
{


class CTenantRelationshipProviderComp:
			public icomp::CComponentBase,
			virtual public ITenantRelationshipProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantRelationshipProviderComp);
		I_REGISTER_INTERFACE(ITenantRelationshipProvider);
		I_ASSIGN(m_connectionRequestManagerCompPtr, "ConnectionRequestManager", "Manager of the tenant connections and relationships", true, "ConnectionRequestManager");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager used to resolve the tenant names", false, "TenantManager");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantRelationshipProvider)
	virtual RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const override;

private:
	QString GetTenantName(const QByteArray& tenantId) const;

private:
	I_REF(ITenantConnectionRequestManager, m_connectionRequestManagerCompPtr);
	I_REF(ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth


