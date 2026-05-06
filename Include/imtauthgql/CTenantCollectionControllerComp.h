// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtgql/CGqlParamObject.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>

// ACF includes
#include <iprm/CParamsSet.h>


namespace imtauthgql
{


class CTenantCollectionControllerComp: public sdl::imtauth::Tenants::CTenantCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Tenants::CTenantCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionControllerComp);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for member count", false, "MembershipManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Tenants::CTenantCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Tenants::CGetTenantListGqlRequest& getTenantListRequest,
				sdl::imtauth::Tenants::CTenantItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

private:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
};


} // namespace imtauthgql
