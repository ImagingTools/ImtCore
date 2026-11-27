// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantDirectory_fwd.h>


namespace imtauthgql
{


/**
	The directory of the organizations on the server of a product, kept by the authorization server.
*/
class CRemoteTenantDirectoryControllerComp: public imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteTenantDirectoryControllerComp);
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
};


} // namespace imtauthgql


