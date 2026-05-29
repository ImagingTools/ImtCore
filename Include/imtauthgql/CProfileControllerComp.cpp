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

// reimplemented (sdl::imtauth::Profile::CGraphQlHandlerCompBase)

sdl::imtauth::Profile::CProfileData CProfileControllerComp::OnGetProfile(
			const sdl::imtauth::Profile::CGetProfileGqlRequest& getProfileRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Profile::CProfileData::V1_0 profileData;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CProfileControllerComp");
		return sdl::imtauth::Profile::CProfileData();
	}

	sdl::imtauth::Profile::GetProfileRequestArguments arguments = getProfileRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Profile::CProfileData();
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
	}

	QByteArray productId;
	if (arguments.input.Version_1_0->productId){
		productId = *arguments.input.Version_1_0->productId;
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get a profile info. Error: User with ID '%1' does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CProfileControllerComp");
		return sdl::imtauth::Profile::CProfileData();
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

	imtsdl::TElementList<sdl::imtauth::Profile::CRoleInfo::V1_0> roleList;

	if (m_roleCollectionCompPtr.IsValid()){
		const QByteArrayList roles = userInfoPtr->GetRoles(productId);

		for (const QByteArray& roleId : std::as_const(roles)){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
				if (roleInfoPtr != nullptr){
					sdl::imtauth::Profile::CRoleInfo::V1_0 info;
					info.id = QByteArray(roleInfoPtr->GetRoleId());
					info.name = QString(roleInfoPtr->GetRoleName());
					info.description = QString(roleInfoPtr->GetRoleDescription());

					roleList << info;
				}
			}
		}
	}

	profileData.roles = std::move(roleList);

	imtsdl::TElementList<sdl::imtauth::Profile::CGroupInfo::V1_0> groupList;

	if (m_groupCollectionCompPtr.IsValid()){
		QByteArrayList groups = userInfoPtr->GetGroups();

		for (const QByteArray& groupId : std::as_const(groups)){
			imtbase::IObjectCollection::DataPtr groupDataPtr;
			if (m_groupCollectionCompPtr->GetObjectData(groupId, groupDataPtr)){
				const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(groupDataPtr.GetPtr());
				if (groupInfoPtr != nullptr){
					sdl::imtauth::Profile::CGroupInfo::V1_0 info;

					info.id = QByteArray(groupInfoPtr->GetId());
					info.name = QString(groupInfoPtr->GetName());
					info.description = QString(groupInfoPtr->GetDescription());

					groupList << info;
				}
			}
		}
	}

	profileData.groups = std::move(groupList);

	imtsdl::TElementList<sdl::imtauth::Profile::CPermissionInfo::V1_0> permissionList;

	QByteArrayList permissions = userInfoPtr->GetPermissions(productId);
	for (const QByteArray& permissionId : std::as_const(permissions)){
		sdl::imtauth::Profile::CPermissionInfo::V1_0 info;
		info.id = permissionId;
		permissionList << info;
	}

	profileData.permissions = std::move(permissionList);

	sdl::imtauth::Profile::CProfileData retVal;
	retVal.Version_1_0 = std::move(profileData);

	return retVal;
}


sdl::imtauth::Profile::CGetUserOrganizationsPayload CProfileControllerComp::OnGetUserOrganizations(
			const sdl::imtauth::Profile::CGetUserOrganizationsGqlRequest& getUserOrganizationsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Profile::CGetUserOrganizationsPayload response;
	response.Version_1_0.emplace();

	sdl::imtauth::Profile::GetUserOrganizationsRequestArguments arguments = getUserOrganizationsRequest.GetRequestedArguments();

	QByteArray userId;
	if (arguments.input.Version_1_0.has_value() && arguments.input.Version_1_0->id){
		userId = *arguments.input.Version_1_0->id;
	}

	if (!userId.isEmpty()){
		response.Version_1_0->organizations = CreateOrganizationList(userId);
	}

	return response;
}


imtsdl::TElementList<sdl::imtauth::Profile::CProfileTenantInfo::V1_0> CProfileControllerComp::CreateOrganizationList(
			const QByteArray& userId) const
{
	imtsdl::TElementList<sdl::imtauth::Profile::CProfileTenantInfo::V1_0> organizationList;

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

		sdl::imtauth::Profile::CProfileTenantInfo::V1_0 organizationInfo;
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


sdl::imtauth::Profile::CSetProfileResponse CProfileControllerComp::OnSetProfile(
			const sdl::imtauth::Profile::CSetProfileGqlRequest& setProfileRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::Profile::CSetProfileResponse::V1_0 response;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CProfileControllerComp");

		return sdl::imtauth::Profile::CSetProfileResponse();
	}

	sdl::imtauth::Profile::SetProfileRequestArguments arguments = setProfileRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Profile::CSetProfileResponse();
	}

	QByteArray id;
	if (arguments.input.Version_1_0->id){
		id = *arguments.input.Version_1_0->id;
	}

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(id, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to set a profile info. Error: User with ID '%1' does not exists").arg(qPrintable(id));
		return sdl::imtauth::Profile::CSetProfileResponse();
	}

	QString name;
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}

	QString email;
	if (arguments.input.Version_1_0->email){
		email = *arguments.input.Version_1_0->email;
	}

	userInfoPtr->SetName(name);
	userInfoPtr->SetMail(email);

	if (!m_userCollectionCompPtr->SetObjectData(id, *userInfoPtr)){
		errorMessage = QString("Unable to set a profile info. Error: User collection cannot to update an object with ID '%1'").arg(qPrintable(id));

		return sdl::imtauth::Profile::CSetProfileResponse();
	}

	response.status = true;

	sdl::imtauth::Profile::CSetProfileResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


} // namespace imtauthgql
