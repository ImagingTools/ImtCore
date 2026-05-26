// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/RoleCollectionDocumentService.h>


namespace imtauthgql
{


/**
 * Client-side proxy that forwards the role-representation GraphQL endpoints
 * to a remote server (analogous to CRemoteTenantCollectionDocumentServiceControllerComp).
 */
class CRemoteRoleCollectionDocumentServiceControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::RoleCollectionDocumentService::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
					sdl::imtauth::RoleCollectionDocumentService::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteRoleCollectionDocumentServiceControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::RoleCollectionDocumentService::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Roles::CRoleData OnGetRoleRepresentation(
				const sdl::imtauth::RoleCollectionDocumentService::CGetRoleRepresentationGqlRequest& getRoleRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateRoleFromRepresentation(
				const sdl::imtauth::RoleCollectionDocumentService::CUpdateRoleFromRepresentationGqlRequest& updateRoleFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql
