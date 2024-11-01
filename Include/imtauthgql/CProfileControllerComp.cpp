#include <imtauthgql/CProfileControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Features.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Profile::V1_0::CGraphQlHandlerCompBase)

sdl::imtauth::Profile::V1_0::CProfileData CProfileControllerComp::OnGetProfile(
			const sdl::imtauth::Profile::V1_0::CGetProfileGqlRequest& getProfileRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::Profile::V1_0::CProfileData profileData;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CProfileControllerComp");
		return profileData;
	}

	QByteArray objectId = getProfileRequest.GetRequestedArguments().input.GetId();
	QByteArray productId = getProfileRequest.GetRequestedArguments().input.GetProductId();

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get a profile info. Error: User with ID '%1' does not exists").arg(qPrintable(objectId));
		return profileData;
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if (systemInfo.enabled){
			profileData.SetSystemId(systemInfo.systemId);
			break;
		}
	}

	profileData.SetId(objectId);
	profileData.SetName(userInfoPtr->GetName());
	profileData.SetEmail(userInfoPtr->GetMail());
	profileData.SetUsername(userInfoPtr->GetId());

	QList<sdl::imtauth::Profile::V1_0::CRoleInfo> roleList;

	if (m_roleCollectionCompPtr.IsValid()){
		QByteArrayList roles = userInfoPtr->GetRoles(productId);

		for (const QByteArray& roleId : roles){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
				if (roleInfoPtr != nullptr){
					sdl::imtauth::Profile::V1_0::CRoleInfo info;
					info.SetId(roleInfoPtr->GetRoleId());
					info.SetName(roleInfoPtr->GetRoleName());
					info.SetDescription(roleInfoPtr->GetRoleDescription());

					roleList << info;
				}
			}
		}
	}

	profileData.SetRoles(roleList);

	QList<sdl::imtauth::Profile::V1_0::CGroupInfo> groupList;

	if (m_groupCollectionCompPtr.IsValid()){
		QByteArrayList groups = userInfoPtr->GetGroups();

		for (const QByteArray& groupId : groups){
			imtbase::IObjectCollection::DataPtr groupDataPtr;
			if (m_groupCollectionCompPtr->GetObjectData(groupId, groupDataPtr)){
				const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
				if (groupInfoPtr != nullptr){
					sdl::imtauth::Profile::V1_0::CGroupInfo info;

					info.SetId(groupInfoPtr->GetId());
					info.SetName(groupInfoPtr->GetName());
					info.SetDescription(groupInfoPtr->GetDescription());

					groupList << info;
				}
			}
		}
	}

	profileData.SetGroups(groupList);

	QList<sdl::imtauth::Profile::V1_0::CPermissionInfo> permissionList;

	if (m_featureCollectionCompPtr.IsValid()){
		QByteArrayList permissions = userInfoPtr->GetPermissions(productId);
		for (imtbase::ICollectionInfo::Id& elementId : m_featureCollectionCompPtr->GetElementIds()){
			imtbase::IObjectCollection::DataPtr groupDataPtr;
			if (m_featureCollectionCompPtr->GetObjectData(elementId, groupDataPtr)){
				const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(groupDataPtr.GetPtr());
				if (featureInfoPtr != nullptr){
					for (imtbase::ICollectionInfo::Id& subFeatureId : featureInfoPtr->GetSubFeatureIds()){
						if (permissions.contains(subFeatureId)){
							const imtlic::IFeatureInfo* subFeatureInfoPtr = featureInfoPtr->GetSubFeature(subFeatureId);
							if (subFeatureInfoPtr != nullptr){
								sdl::imtauth::Profile::V1_0::CPermissionInfo info;

								info.SetId(subFeatureInfoPtr->GetFeatureId());
								info.SetName(subFeatureInfoPtr->GetFeatureName());
								info.SetDescription(subFeatureInfoPtr->GetFeatureDescription());

								permissionList << info;
							}
						}
					}
				}
			}
		}
	}

	profileData.SetPermissions(permissionList);

	return profileData;
}


sdl::imtauth::Profile::V1_0::CSetProfileResponse CProfileControllerComp::OnSetProfile(
			const sdl::imtauth::Profile::V1_0::CSetProfileGqlRequest& setProfileRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::Profile::V1_0::CSetProfileResponse response;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CProfileControllerComp");
		return response;
	}

	QByteArray id = setProfileRequest.GetRequestedArguments().input.GetId();

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(id, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to set a profile info. Error: User with ID '%1' does not exists").arg(qPrintable(id));
		return response;
	}

	QString name = setProfileRequest.GetRequestedArguments().input.GetName();
	QString email = setProfileRequest.GetRequestedArguments().input.GetEmail();

	userInfoPtr->SetName(name);
	userInfoPtr->SetMail(email);

	if (!m_userCollectionCompPtr->SetObjectData(id, *userInfoPtr)){
		errorMessage = QString("Unable to set a profile info. Error: User collection cannot to update an object with ID '%1'").arg(qPrintable(id));
		return response;
	}

	response.SetStatus(true);

	return response;
}


} // namespace imtauthgql


