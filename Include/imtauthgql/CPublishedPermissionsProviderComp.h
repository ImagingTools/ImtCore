// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ITenantManager.h>
#include <imtlic/IProductInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Permissions_fwd.h>


namespace imtauthgql
{


/**
	Answers the requests for a permission tree on the authorization server, from the trees the products published.
	The contract is the one of CPermissionsProviderComp on the server of a product.
	The permissions of this server the roles of a product hold too are added to its tree.
*/
class CPublishedPermissionsProviderComp: public sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPublishedPermissionsProviderComp);
		I_ASSIGN(m_snapshotCollectionCompPtr, "SnapshotCollection", "Published permission trees", true, "SnapshotCollection");
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Permissions of this server given to the roles of the products", false, "PermissionTree");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Limits a tree to the permission scope of an organization", false, "TenantManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetProductPermissionsPayload OnGetProductPermissions(
				const sdl::V1_0::imtauth::CGetProductPermissionsGqlRequest& getProductPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetProductPermissionsPayload OnGetUserPermissions(
				const sdl::V1_0::imtauth::CGetUserPermissionsGqlRequest& getUserPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	sdl::V1_0::imtauth::CGetProductPermissionsPayload CreatePayload(
				const QByteArray& productId,
				const QByteArray& tenantId,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;

private:
	I_REF(imtbase::IObjectCollection, m_snapshotCollectionCompPtr);
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauthgql
