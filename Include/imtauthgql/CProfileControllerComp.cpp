// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CProfileControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile.h>


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

// reimplemented (sdl::V1_0::imtauth::CProfileGqlHandlerCompBase)

sdl::V1_0::imtauth::CProfileData CProfileControllerComp::OnGetProfile(
			const sdl::V1_0::imtauth::CGetProfileGqlRequest& getProfileRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CProfileData profileData;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CProfileControllerComp");
		return sdl::V1_0::imtauth::CProfileData();
	}

	sdl::V1_0::imtauth::GetProfileRequestArguments arguments = getProfileRequest.GetRequestedArguments();

	QByteArray objectId;
	if (arguments.input.id){
		objectId = *arguments.input.id;
	}

	QByteArray productId;
	if (arguments.input.productId){
		productId = *arguments.input.productId;
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get a profile info. Error: User with ID '%1' does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CProfileControllerComp");
		return sdl::V1_0::imtauth::CProfileData();
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : std::as_const(systemInfoList)){
		if (systemInfo.enabled){
			profileData.systemId = QByteArray(systemInfo.systemId);
			break;
		}
	}

	profileData.id = QByteArray(objectId);
	profileData.name = QString(userInfoPtr->GetName());
	profileData.email = QString(userInfoPtr->GetMail());
	profileData.username = QString(userInfoPtr->GetId());

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		profileData.currentTenantId = gqlContextPtr->GetTenantId();
	}

	imtsdl::TElementList<sdl::V1_0::imtauth::CRoleInfo> roleList;

	if (m_roleCollectionCompPtr.IsValid()){
		const QByteArrayList roles = userInfoPtr->GetRoles(productId);

		for (const QByteArray& roleId : std::as_const(roles)){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
				if (roleInfoPtr != nullptr){
					sdl::V1_0::imtauth::CRoleInfo info;
					info.id = QByteArray(roleInfoPtr->GetRoleId());
					info.name = QString(roleInfoPtr->GetRoleName());
					info.description = QString(roleInfoPtr->GetRoleDescription());

					roleList << info;
				}
			}
		}
	}

	profileData.roles = std::move(roleList);

	imtsdl::TElementList<sdl::V1_0::imtauth::CGroupInfo> groupList;

	if (m_groupCollectionCompPtr.IsValid()){
		QByteArrayList groups = userInfoPtr->GetGroups();

		for (const QByteArray& groupId : std::as_const(groups)){
			imtbase::IObjectCollection::DataPtr groupDataPtr;
			if (m_groupCollectionCompPtr->GetObjectData(groupId, groupDataPtr)){
				const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
				if (groupInfoPtr != nullptr){
					sdl::V1_0::imtauth::CGroupInfo info;

					info.id = QByteArray(groupInfoPtr->GetId());
					info.name = QString(groupInfoPtr->GetName());
					info.description = QString(groupInfoPtr->GetDescription());

					groupList << info;
				}
			}
		}
	}

	profileData.groups = std::move(groupList);

	imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionInfo> permissionList;

	QByteArrayList permissions = userInfoPtr->GetPermissions(productId);
	for (const QByteArray& permissionId : std::as_const(permissions)){
		sdl::V1_0::imtauth::CPermissionInfo info;
		info.id = permissionId;
		permissionList << info;
	}

	profileData.permissions = std::move(permissionList);

	return profileData;
}


sdl::V1_0::imtauth::CGetUserOrganizationsPayload CProfileControllerComp::OnGetUserOrganizations(
			const sdl::V1_0::imtauth::CGetUserOrganizationsGqlRequest& getUserOrganizationsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetUserOrganizationsPayload response;
	sdl::V1_0::imtauth::GetUserOrganizationsRequestArguments arguments = getUserOrganizationsRequest.GetRequestedArguments();

	QByteArray userId;
	if (arguments.input.id){
		userId = *arguments.input.id;
	}

	if (!userId.isEmpty()){
		response.organizations = CreateOrganizationList(userId);
	}

	return response;
}


imtsdl::TElementList<sdl::V1_0::imtauth::CProfileTenantInfo> CProfileControllerComp::CreateOrganizationList(
			const QByteArray& userId) const
{
	imtsdl::TElementList<sdl::V1_0::imtauth::CProfileTenantInfo> organizationList;

	if (!m_tenantManagerCompPtr.IsValid()){
		return organizationList;
	}

	for (const QByteArray& tenantId : m_tenantManagerCompPtr->GetTenantIds()){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
		if (!tenantPtr.IsValid() || !tenantPtr->IsActive()){
			continue;
		}

		const bool isOwner = tenantPtr->GetOwnerId() == userId;
		imtauth::ITenantMembershipUniquePtr membershipPtr;
		if (m_membershipManagerCompPtr.IsValid()){
			membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId);
		}

		const bool isActiveMember = membershipPtr.IsValid() && membershipPtr->IsActive();
		if (!isOwner && !isActiveMember){
			continue;
		}

		sdl::V1_0::imtauth::CProfileTenantInfo organizationInfo;
		organizationInfo.id = tenantId;
		organizationInfo.name = tenantPtr->GetTenantName();
		organizationInfo.description = tenantPtr->GetTenantDescription();
		organizationInfo.ownerId = tenantPtr->GetOwnerId();
		organizationInfo.isOwner = isOwner;
		organizationInfo.isActive = tenantPtr->IsActive();
		if (isOwner){
			organizationInfo.role = QStringLiteral("Owner");
		}
		else{
			organizationInfo.role = TenantMembershipRoleToString(membershipPtr->GetRoleId());
		}

		organizationList << organizationInfo;
	}

	return organizationList;
}


QString CProfileControllerComp::TenantMembershipRoleToString(const QByteArray& roleId) const
{
	return QString::fromUtf8(roleId);
}


sdl::V1_0::imtauth::CSetProfileResponse CProfileControllerComp::OnSetProfile(
			const sdl::V1_0::imtauth::CSetProfileGqlRequest& setProfileRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CSetProfileResponse response;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CProfileControllerComp");

		return sdl::V1_0::imtauth::CSetProfileResponse();
	}

	sdl::V1_0::imtauth::SetProfileRequestArguments arguments = setProfileRequest.GetRequestedArguments();
	QByteArray id;
	if (arguments.input.id){
		id = *arguments.input.id;
	}

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(id, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to set a profile info. Error: User with ID '%1' does not exists").arg(qPrintable(id));
		return sdl::V1_0::imtauth::CSetProfileResponse();
	}

	QString name;
	if (arguments.input.name){
		name = *arguments.input.name;
	}

	QString email;
	if (arguments.input.email){
		email = *arguments.input.email;
	}

	userInfoPtr->SetName(name);
	userInfoPtr->SetMail(email);

	if (!m_userCollectionCompPtr->SetObjectData(id, *userInfoPtr)){
		errorMessage = QString("Unable to set a profile info. Error: User collection cannot to update an object with ID '%1'").arg(qPrintable(id));

		return sdl::V1_0::imtauth::CSetProfileResponse();
	}

	response.status = true;

	return response;
}


} // namespace imtauthgql
