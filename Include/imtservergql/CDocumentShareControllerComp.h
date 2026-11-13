// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IDocumentShareManager.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentShares_fwd.h>


namespace imtservergql
{


/**
	GraphQL controller for the document shares of the requesting organization across all collections.
	The owner organization lists and removes its outgoing shares; the target organization lists its incoming
	shares, accepts or declines them and restricts them to its own user groups.
*/
class CDocumentShareControllerComp: public sdl::V1_0::imtbase::CDocumentSharesGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CDocumentSharesGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentShareControllerComp);
		I_ASSIGN(m_documentShareManagerCompPtr, "DocumentShareManager", "Manager of the document shares", true, "DocumentShareManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CDocumentSharesGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CGetOrganizationDocumentSharesPayload OnGetOrganizationDocumentShares(
				const sdl::V1_0::imtbase::CGetOrganizationDocumentSharesGqlRequest& getOrganizationDocumentSharesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CSetDocumentSharesStatusPayload OnSetDocumentSharesStatus(
				const sdl::V1_0::imtbase::CSetDocumentSharesStatusGqlRequest& setDocumentSharesStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CSetDocumentShareGroupsPayload OnSetDocumentShareGroups(
				const sdl::V1_0::imtbase::CSetDocumentShareGroupsGqlRequest& setDocumentShareGroupsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRemoveOrganizationDocumentSharesPayload OnRemoveOrganizationDocumentShares(
				const sdl::V1_0::imtbase::CRemoveOrganizationDocumentSharesGqlRequest& removeOrganizationDocumentSharesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	enum ShareSide
	{
		SSI_OWNER,
		SSI_TARGET
	};

	/**
		Check that every share belongs to the requesting organization on the given side.
	*/
	bool CheckShareSide(const imtgql::CGqlRequest& gqlRequest, const QByteArrayList& shareIds, ShareSide side, QString& errorMessage) const;

private:
	I_REF(imtauth::IDocumentShareManager, m_documentShareManagerCompPtr);
};


} // namespace imtservergql


