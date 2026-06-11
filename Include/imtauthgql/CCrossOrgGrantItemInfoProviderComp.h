// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtservergql/ISelectableItemInfoProvider.h>


namespace imtauthgql
{


/**
	Provides additional parameters (target tenant name, roles, expiry)
	for cross-org grant items displayed in FilterableSelect lists.

	\ingroup FilterableSelect
*/
class CCrossOrgGrantItemInfoProviderComp:
		public imtservergql::ISelectableItemInfoProvider
{
public:
	I_BEGIN_COMPONENT(CCrossOrgGrantItemInfoProviderComp);
		I_ASSIGN(m_grantCollectionCompPtr, "GrantCollection", "Cross-org grant collection", true, "GrantCollection");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for name resolution", false, "TenantCollection");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection for name resolution", false, "RoleCollection");
	I_END_COMPONENT;

	// reimplemented (ISelectableItemInfoProvider)
	virtual bool GetItemParameters(
		const QByteArray& objectId,
		imtsdl::TElementList<sdl::V1_0::imtbase::CParameter>& parameters) const override;

private:
	I_REF(imtbase::IObjectCollection, m_grantCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
};


} // namespace imtauthgql
