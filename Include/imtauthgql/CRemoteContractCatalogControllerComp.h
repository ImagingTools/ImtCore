// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ContractCatalogs_fwd.h>


namespace imtauthgql
{


/**
	The roles of the parties and the types of contracts on the server of a product, kept by the authorization server.
*/
class CRemoteContractCatalogControllerComp: public imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteContractCatalogControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetPartyRolesPayload OnGetPartyRoles(
				const sdl::V1_0::imtauth::CGetPartyRolesGqlRequest& getPartyRolesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractTypesPayload OnGetContractTypes(
				const sdl::V1_0::imtauth::CGetContractTypesGqlRequest& getContractTypesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSavePartyRole(
				const sdl::V1_0::imtauth::CSavePartyRoleGqlRequest& savePartyRoleRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSetPartyRoleArchived(
				const sdl::V1_0::imtauth::CSetPartyRoleArchivedGqlRequest& setPartyRoleArchivedRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSaveContractType(
				const sdl::V1_0::imtauth::CSaveContractTypeGqlRequest& saveContractTypeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCatalogCommandPayload OnSetContractTypeArchived(
				const sdl::V1_0::imtauth::CSetContractTypeArchivedGqlRequest& setContractTypeArchivedRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


