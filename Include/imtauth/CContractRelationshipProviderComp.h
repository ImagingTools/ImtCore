// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContractProvider.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantRelationshipProvider.h>


namespace imtauth
{


/**
	The relationships of the organizations as the links of their contracts show them.
*/
class CContractRelationshipProviderComp:
			public icomp::CComponentBase,
			virtual public ITenantRelationshipProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractRelationshipProviderComp);
		I_REGISTER_INTERFACE(ITenantRelationshipProvider);
		I_ASSIGN(m_contractProviderCompPtr, "ContractProvider", "Contracts of the organizations", true, "ContractProvider");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Current names of the organizations", false, "TenantManager");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantRelationshipProvider)
	virtual RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const override;

private:
	I_REF(IContractProvider, m_contractProviderCompPtr);
	I_REF(ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth


