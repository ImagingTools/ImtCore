// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteProfileControllerComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Features.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::Profile::CGraphQlHandlerCompBase)

sdl::imtauth::Profile::CProfileData CRemoteProfileControllerComp::OnGetProfile(
			const sdl::imtauth::Profile::CGetProfileGqlRequest& /*getProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Profile::CProfileData profileData =
				SendModelRequest<sdl::imtauth::Profile::CProfileData>(gqlRequest, errorMessage);

	if (!profileData.Version_1_0.has_value()){
		return profileData;
	}

	// Supplement permissions from local ProductInfo
	if (m_productInfoCompPtr.IsValid()){
		imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
		if (featureCollectionPtr != nullptr){
			QByteArrayList permissions;
			if (profileData.Version_1_0->permissions.has_value()){
				for (const auto& permInfo : *profileData.Version_1_0->permissions){
					if (permInfo->id){
						permissions.append(*permInfo->id);
					}
				}
			}

			imtsdl::TElementList<sdl::imtauth::Profile::CPermissionInfo::V1_0> permissionList;

			for (imtbase::ICollectionInfo::Id& elementId : featureCollectionPtr->GetElementIds()){
				imtbase::IObjectCollection::DataPtr permissionDataPtr;
				if (featureCollectionPtr->GetObjectData(elementId, permissionDataPtr)){
					const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(permissionDataPtr.GetPtr());
					if (featureInfoPtr != nullptr){
						for (imtbase::ICollectionInfo::Id& subFeatureId : featureInfoPtr->GetSubFeatureIds()){
							if (permissions.contains(subFeatureId)){
								imtlic::IFeatureInfoSharedPtr subFeatureInfoPtr = featureInfoPtr->GetSubFeature(subFeatureId);
								if (subFeatureInfoPtr.IsValid()){
									sdl::imtauth::Profile::CPermissionInfo::V1_0 info;

									info.id = QByteArray(subFeatureInfoPtr->GetFeatureId());
									info.name = QString(subFeatureInfoPtr->GetFeatureName());
									info.description = QString(subFeatureInfoPtr->GetFeatureDescription());

									permissionList << info;
								}
							}
						}
					}
				}
			}

			profileData.Version_1_0->permissions = std::move(permissionList);
		}
	}

	return profileData;
}


sdl::imtauth::Profile::CSetProfileResponse CRemoteProfileControllerComp::OnSetProfile(
			const sdl::imtauth::Profile::CSetProfileGqlRequest& /*setProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Profile::CSetProfileResponse>(gqlRequest, errorMessage);
}


sdl::imtauth::Profile::CGetUserOrganizationsPayload CRemoteProfileControllerComp::OnGetUserOrganizations(
			const sdl::imtauth::Profile::CGetUserOrganizationsGqlRequest& /*getUserOrganizationsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Profile::CGetUserOrganizationsPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
