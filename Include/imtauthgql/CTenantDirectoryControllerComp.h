// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantDirectory.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantDirectory_fwd.h>


namespace imtauthgql
{


/**
	The directory of the organizations: an organization lists itself to be found by unknown partners.
*/
class CTenantDirectoryControllerComp: public sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantDirectoryControllerComp);
		I_ASSIGN(m_directoryCompPtr, "TenantDirectory", "Directory of the organizations", true, "TenantDirectory");
		I_ASSIGN(m_profileFactoryCompPtr, "ProfileFactory", "Factory of a profile", true, "TenantDirectoryProfile");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Members and their permissions in the organizations", true, "TenantMembershipManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetDirectoryProfilePayload OnGetDirectoryProfile(
				const sdl::V1_0::imtauth::CGetDirectoryProfileGqlRequest& getDirectoryProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSetDirectoryProfilePayload OnSetDirectoryProfile(
				const sdl::V1_0::imtauth::CSetDirectoryProfileGqlRequest& setDirectoryProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantDirectory, m_directoryCompPtr);
	I_FACT(imtauth::ITenantDirectoryProfile, m_profileFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
};


} // namespace imtauthgql


