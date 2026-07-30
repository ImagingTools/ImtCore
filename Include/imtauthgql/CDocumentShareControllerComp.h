// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IDocumentShare.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/DocumentShares_fwd.h>


namespace imtauthgql
{


/**
	GraphQL handler of the document share entity.

	Exposes the IDocumentShareManager operations (create, list, change access
	mode, revoke and access resolution) as GraphQL queries and mutations of the
	DocumentShares schema.
*/
class CDocumentShareControllerComp:
			public sdl::V1_0::imtauth::CDocumentSharesGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CDocumentSharesGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentShareControllerComp);
		I_ASSIGN(m_shareManagerCompPtr, "DocumentShareManager", "Manager of document shares", true, "DocumentShareManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CDocumentSharesGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetDocumentSharePayload OnGetDocumentShare(
				const sdl::V1_0::imtauth::CGetDocumentShareGqlRequest& getDocumentShareRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetDocumentSharesPayload OnGetDocumentShares(
				const sdl::V1_0::imtauth::CGetDocumentSharesGqlRequest& getDocumentSharesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetDocumentAccessPayload OnGetDocumentAccess(
				const sdl::V1_0::imtauth::CGetDocumentAccessGqlRequest& getDocumentAccessRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CShareDocumentPayload OnShareDocument(
				const sdl::V1_0::imtauth::CShareDocumentGqlRequest& shareDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSetDocumentShareAccessModePayload OnSetDocumentShareAccessMode(
				const sdl::V1_0::imtauth::CSetDocumentShareAccessModeGqlRequest& setDocumentShareAccessModeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeDocumentSharePayload OnRevokeDocumentShare(
				const sdl::V1_0::imtauth::CRevokeDocumentShareGqlRequest& revokeDocumentShareRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::IDocumentShareManager, m_shareManagerCompPtr);
};


} // namespace imtauthgql
