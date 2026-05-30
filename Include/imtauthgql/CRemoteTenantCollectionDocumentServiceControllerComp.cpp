// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantCollectionDocumentServiceControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentService.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CTenantData CRemoteTenantCollectionDocumentServiceControllerComp::OnGetTenantRepresentation(
		const sdl::V1_0::imtauth::CGetTenantRepresentationGqlRequest& /*getTenantRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::CTenantData response = SendModelRequest<sdl::V1_0::imtauth::CTenantData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return response;
	}

	if (!response.HasValue()){
		return response;
	}

	// Enrich response with allProductPermissions as tree from local IProductInfo
	response.allProductPermissions.Emplace();
	if (m_productInfoCompPtr.IsValid()){
		imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
		if (featureCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
			for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
					const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
					if (featureInfoPtr != nullptr && featureInfoPtr->IsPermission()){
						CollectPermissionsTree(featureInfoPtr, *response.allProductPermissions);
					}
				}
			}
		}
	}

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRemoteTenantCollectionDocumentServiceControllerComp::OnUpdateTenantFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateTenantFromRepresentationGqlRequest& /*updateTenantFromRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


// private methods

void CRemoteTenantCollectionDocumentServiceControllerComp::CollectPermissionsTree(
		const imtlic::IFeatureInfo* featureInfoPtr,
		imtsdl::TElementList<sdl::V1_0::imtauth::CTenantPermissionOption>& permissions) const
{
	if (featureInfoPtr == nullptr){
		return;
	}

	sdl::V1_0::imtauth::CTenantPermissionOption permOpt;
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
