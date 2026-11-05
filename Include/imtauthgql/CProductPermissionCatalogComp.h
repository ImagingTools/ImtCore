// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ProductPermissionCatalog_fwd.h>


namespace imtauthgql
{


/**
	Stores the permission trees the servers of products publish.
*/
class CProductPermissionCatalogComp: public sdl::V1_0::imtauth::CProductPermissionCatalogGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CProductPermissionCatalogGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionCatalogComp);
		I_ASSIGN(m_snapshotCollectionCompPtr, "SnapshotCollection", "Published permission trees", true, "SnapshotCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CProductPermissionCatalogGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CPublishProductPermissionsPayload OnPublishProductPermissions(
				const sdl::V1_0::imtauth::CPublishProductPermissionsGqlRequest& publishRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CProductPermissionStatus OnGetProductPermissionStatus(
				const sdl::V1_0::imtauth::CGetProductPermissionStatusGqlRequest& statusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_snapshotCollectionCompPtr);
};


} // namespace imtauthgql
