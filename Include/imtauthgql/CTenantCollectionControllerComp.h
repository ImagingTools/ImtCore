// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantInvitationManager.h>
#include <imtauthsdl/SDL/1.0/CPP/Tenants_fwd.h>


namespace imtauthgql
{


class CTenantCollectionControllerComp: public sdl::V1_0::imtauth::CTenantCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CTenantCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionControllerComp);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for member count", false, "MembershipManager");
		I_ASSIGN(m_invitationManagerCompPtr, "InvitationManager", "Tenant invitation manager for invitation lookup", false, "InvitationManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CTenantCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtauth::CGetTenantListGqlRequest& getTenantListRequest,
				sdl::V1_0::imtauth::CTenantItemData& representationObject,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

private:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ITenantInvitationManager, m_invitationManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql
