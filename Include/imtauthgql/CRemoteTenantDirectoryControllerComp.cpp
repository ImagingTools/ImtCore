// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantDirectoryControllerComp.h>


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantDirectory.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetDirectoryProfilePayload CRemoteTenantDirectoryControllerComp::OnGetDirectoryProfile(
			const sdl::V1_0::imtauth::CGetDirectoryProfileGqlRequest& /*getDirectoryProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetDirectoryProfilePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CSetDirectoryProfilePayload CRemoteTenantDirectoryControllerComp::OnSetDirectoryProfile(
			const sdl::V1_0::imtauth::CSetDirectoryProfileGqlRequest& /*setDirectoryProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSetDirectoryProfilePayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


