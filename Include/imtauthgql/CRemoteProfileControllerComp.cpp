// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile.h>
#include <imtauthgql/CRemoteProfileControllerComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Features.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CProfileGqlHandlerCompBase)

sdl::V1_0::imtauth::CProfileData CRemoteProfileControllerComp::OnGetProfile(
			const sdl::V1_0::imtauth::CGetProfileGqlRequest& /*getProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CProfileData profileData =
				SendModelRequest<sdl::V1_0::imtauth::CProfileData>(gqlRequest, errorMessage);

	// Supplement permissions from local ProductInfo
	if (m_productInfoCompPtr.IsValid()){
		imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
		if (featureCollectionPtr != nullptr){
			QByteArrayList permissions;
			if (profileData.permissions.has_value()){
				for (const auto& permInfo : *profileData.permissions){
					if (permInfo->id){
						permissions.append(*permInfo->id);
					}
				}
			}

			imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionInfo> permissionList;

			for (imtbase::ICollectionInfo::Id& elementId : featureCollectionPtr->GetElementIds()){
				imtbase::IObjectCollection::DataPtr permissionDataPtr;
				if (featureCollectionPtr->GetObjectData(elementId, permissionDataPtr)){
					const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(permissionDataPtr.GetPtr());
					if (featureInfoPtr != nullptr){
						for (imtbase::ICollectionInfo::Id& subFeatureId : featureInfoPtr->GetSubFeatureIds()){
							if (permissions.contains(subFeatureId)){
								imtlic::IFeatureInfoSharedPtr subFeatureInfoPtr = featureInfoPtr->GetSubFeature(subFeatureId);
								if (subFeatureInfoPtr.IsValid()){
									sdl::V1_0::imtauth::CPermissionInfo info;

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

			profileData.permissions = std::move(permissionList);
		}
	}

	return profileData;
}


sdl::V1_0::imtauth::CSetProfileResponse CRemoteProfileControllerComp::OnSetProfile(
			const sdl::V1_0::imtauth::CSetProfileGqlRequest& /*setProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSetProfileResponse>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetUserOrganizationsPayload CRemoteProfileControllerComp::OnGetUserOrganizations(
			const sdl::V1_0::imtauth::CGetUserOrganizationsGqlRequest& /*getUserOrganizationsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetUserOrganizationsPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
