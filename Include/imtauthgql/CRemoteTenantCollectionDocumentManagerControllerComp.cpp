// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantCollectionDocumentManagerControllerComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CTenantData CRemoteTenantCollectionDocumentManagerControllerComp::OnGetTenantRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CGetTenantRepresentationGqlRequest& /*getTenantRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::Tenants::CTenantData response = SendModelRequest<sdl::imtauth::Tenants::CTenantData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return response;
	}

	if (!response.Version_1_0.HasValue()){
		return response;
	}

	// Enrich response with allProductPermissions as tree from local IProductInfo
	response.Version_1_0->allProductPermissions.Emplace();
	if (m_productInfoCompPtr.IsValid()){
		imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
		if (featureCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
			for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
					const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
					if (featureInfoPtr != nullptr && featureInfoPtr->IsPermission()){
						CollectPermissionsTree(featureInfoPtr, *response.Version_1_0->allProductPermissions);
					}
				}
			}
		}
	}

	return response;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CRemoteTenantCollectionDocumentManagerControllerComp::OnUpdateTenantFromRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CUpdateTenantFromRepresentationGqlRequest& /*updateTenantFromRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


// private methods

void CRemoteTenantCollectionDocumentManagerControllerComp::CollectPermissionsTree(
		const imtlic::IFeatureInfo* featureInfoPtr,
		imtsdl::TElementList<sdl::imtauth::Tenants::CTenantPermissionOption::V1_0>& permissions) const
{
	if (featureInfoPtr == nullptr){
		return;
	}

	sdl::imtauth::Tenants::CTenantPermissionOption::V1_0 permOpt;
	permOpt.id = featureInfoPtr->GetFeatureId();
	permOpt.name = featureInfoPtr->GetFeatureName();
	permOpt.description = featureInfoPtr->GetFeatureDescription();

	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
	if (!subFeatures.isEmpty()){
		permOpt.children.Emplace();
		for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : subFeatures){
			if (subFeaturePtr.IsValid()){
				CollectPermissionsTree(subFeaturePtr.GetPtr(), *permOpt.children);
			}
		}
	}

	permissions.append(permOpt);
}


} // namespace imtauthgql
