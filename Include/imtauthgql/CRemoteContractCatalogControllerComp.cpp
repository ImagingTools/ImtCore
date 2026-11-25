// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteContractCatalogControllerComp.h>


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ContractCatalogs.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CContractCatalogsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetPartyRolesPayload CRemoteContractCatalogControllerComp::OnGetPartyRoles(
			const sdl::V1_0::imtauth::CGetPartyRolesGqlRequest& /*getPartyRolesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetPartyRolesPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetContractTypesPayload CRemoteContractCatalogControllerComp::OnGetContractTypes(
			const sdl::V1_0::imtauth::CGetContractTypesGqlRequest& /*getContractTypesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractTypesPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCatalogCommandPayload CRemoteContractCatalogControllerComp::OnSavePartyRole(
			const sdl::V1_0::imtauth::CSavePartyRoleGqlRequest& /*savePartyRoleRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCatalogCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCatalogCommandPayload CRemoteContractCatalogControllerComp::OnSetPartyRoleArchived(
			const sdl::V1_0::imtauth::CSetPartyRoleArchivedGqlRequest& /*setPartyRoleArchivedRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCatalogCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCatalogCommandPayload CRemoteContractCatalogControllerComp::OnSaveContractType(
			const sdl::V1_0::imtauth::CSaveContractTypeGqlRequest& /*saveContractTypeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCatalogCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCatalogCommandPayload CRemoteContractCatalogControllerComp::OnSetContractTypeArchived(
			const sdl::V1_0::imtauth::CSetContractTypeArchivedGqlRequest& /*setContractTypeArchivedRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCatalogCommandPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


