// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CProfileControllerComp.h>
#include <imtauthgql/imtauthgql.h>
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
	if (arguments.input->id){
		objectId = *arguments.input->id;
	}

	QByteArray productId;
	if (arguments.input->productId){
		productId = *arguments.input->productId;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

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

	// Apply tenant-based adaptation to the user data (roles/groups/permissions
	// + delegated role enrichment). This is the same logic used by
	// CUserCollectionControllerComp::CreateAdaptedObjectData.
	// We obtain an (optionally cloned) version whose GetRoles/GetGroups/GetPermissions
	// already reflect only what is visible for the current tenant (or globals
	// for No Organization).
	QByteArray currentTenantId;
	if (gqlContextPtr != nullptr){
		currentTenantId = gqlContextPtr->GetTenantId();
	}
	QByteArray currentProductId = productId;

	imtauth::ITenantEntityBindingManager* bindingPtr = m_bindingManagerCompPtr.IsValid() ? m_bindingManagerCompPtr.GetPtr() : nullptr;
	imtauth::IDelegatedAccess* delPtr = m_delegatedAccessCompPtr.IsValid() ? m_delegatedAccessCompPtr.GetPtr() : nullptr;
	imtauth::ITenantMembershipManager* memPtr = m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr;
	imtauth::IRoleInfoProvider* roleProvPtr = m_roleInfoProviderCompPtr.IsValid() ? m_roleInfoProviderCompPtr.GetPtr() : nullptr;

	istd::IChangeableUniquePtr adaptedPtr = AdaptUserForTenant(
				objectId,
				*userInfoPtr,
				currentTenantId,
				currentProductId,
				bindingPtr,
				delPtr,
				memPtr,
				roleProvPtr);

	const imtauth::IUserInfo* effectiveUserPtr = userInfoPtr;
	if (adaptedPtr.IsValid()){
		imtauth::IUserInfo* au = dynamic_cast<imtauth::IUserInfo*>(adaptedPtr.GetPtr());
		if (au != nullptr){
			effectiveUserPtr = au;
		}
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

	if (gqlContextPtr != nullptr){
		profileData.currentTenantId = gqlContextPtr->GetTenantId();
	}

	// =========================================================================
	// TENANT-SCOPED PROFILE DATA (Roles / Permissions / Groups)
	// =========================================================================
	//
	// The goal of the logic below is to make the data returned by GetProfile
	// depend on the currently selected tenant (including the special "No
	// Organization" case when tenantId is empty).
	//
	// Requirements:
	//   - When a real tenant is selected → return only Roles, Permissions and
	//     Groups that are relevant inside that tenant.
	//   - When "No Organization" is selected (empty tenantId) → return ONLY
	//     items that are available to the user OUTSIDE of any tenant
	//     (both direct organizations and delegated/foreign organizations).
	//
	// How it works:
	// The actual work is delegated to the shared AdaptUserForTenant helper
	// (extracted to imtauthgql.h and also used by CUserCollectionControllerComp).
	// It performs binding-based filtering for the current tenant (or globals)
	// plus delegated role enrichment.
	// =========================================================================

	imtsdl::TElementList<sdl::V1_0::imtauth::CRoleInfo> roleList;

	if (m_roleCollectionCompPtr.IsValid()){
		// Use the (possibly tenant-adapted) roles list. The adaptation has
		// already filtered according to tenant bindings and appended any
		// delegated roles.
		const QByteArrayList roles = effectiveUserPtr->GetRoles(currentProductId);

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
		// Groups from the (tenant-adapted) user. The AdaptUserForTenant
		// has already applied binding-based filtering for the current tenant
		// (or globals for No Organization).
		QByteArrayList groups = effectiveUserPtr->GetGroups();

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

	// Permissions from the adapted user (consistent with the binding-based
	// tenant adaptation used for user collection views).
	QByteArrayList permissions = effectiveUserPtr->GetPermissions(currentProductId);
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
	if (arguments.input->id){
		userId = *arguments.input->id;
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

	QByteArrayList directTenantIds;

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

		directTenantIds.append(tenantId);

		sdl::V1_0::imtauth::CProfileTenantInfo organizationInfo;
		organizationInfo.id = tenantId;
		organizationInfo.name = tenantPtr->GetTenantName();
		organizationInfo.description = tenantPtr->GetTenantDescription();
		organizationInfo.ownerId = tenantPtr->GetOwnerId();
		organizationInfo.isOwner = isOwner;
		organizationInfo.isActive = tenantPtr->IsActive();
		organizationInfo.isDelegated = false;
		if (isOwner){
			organizationInfo.role = QStringLiteral("Owner");
		}
		else{
			organizationInfo.role = QStringLiteral("Member");
		}

		organizationList << organizationInfo;
	}

	// Append delegated organizations via cross-org grants
	if (m_crossOrgGrantCompPtr.IsValid() && m_membershipManagerCompPtr.IsValid()){
		// Find the user's home tenant(s) and check for grants targeting them
		const imtauth::ITenantMembershipManager::MembershipIds membershipIds =
			m_membershipManagerCompPtr->GetMembershipsByUser(userId);

		QByteArrayList homeTenantIds;
		for (const QByteArray& membershipId : membershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid()){
				QByteArray tenantId = membershipPtr->GetTenantId();
				if (!homeTenantIds.contains(tenantId)){
					homeTenantIds.append(tenantId);
				}
			}
		}

		for (const QByteArray& homeTenantId : homeTenantIds){
			const imtauth::CrossOrgGrants grants = m_crossOrgGrantCompPtr->GetGrantsByTargetTenant(homeTenantId);
			for (const imtauth::CrossOrgGrantInfo& grant : grants){
				if (!grant.isActive){
					continue;
				}
				QByteArray sourceTenantId = grant.sourceTenantId;
				if (directTenantIds.contains(sourceTenantId)){
					continue;
				}

				// Verify at least one role is effective
				bool isEffective = false;
				for (const QByteArray& roleId : grant.roleIds){
					if (m_crossOrgGrantCompPtr->HasAccess(sourceTenantId, homeTenantId, roleId)){
						isEffective = true;
						break;
					}
				}
				if (!isEffective && !grant.roleIds.isEmpty()){
					continue;
				}

				// Check if already added from another home tenant
				bool alreadyAdded = false;
				for (int i = 0; i < organizationList.size(); ++i){
					if (organizationList[i]->id.HasValue() && organizationList[i]->id == sourceTenantId){
						alreadyAdded = true;
						break;
					}
				}
				if (alreadyAdded){
					continue;
				}

				imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(sourceTenantId);
				if (!tenantPtr.IsValid() || !tenantPtr->IsActive()){
					continue;
				}

				sdl::V1_0::imtauth::CProfileTenantInfo organizationInfo;
				organizationInfo.id = sourceTenantId;
				organizationInfo.name = tenantPtr->GetTenantName();
				organizationInfo.description = tenantPtr->GetTenantDescription();
				organizationInfo.ownerId = tenantPtr->GetOwnerId();
				organizationInfo.isOwner = false;
				organizationInfo.isActive = tenantPtr->IsActive();
				organizationInfo.isDelegated = true;
				organizationInfo.role = QStringLiteral("Delegated Member");

				imtsdl::TElementList<QString> delegatedRolesList;
				for (const QByteArray& roleId : grant.roleIds){
					delegatedRolesList << QString::fromUtf8(roleId);
				}
				organizationInfo.delegatedRoles = delegatedRolesList;

				organizationList << organizationInfo;
			}
		}
	}

	return organizationList;
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
	if (arguments.input->id){
		id = *arguments.input->id;
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
	if (arguments.input->name){
		name = *arguments.input->name;
	}

	QString email;
	if (arguments.input->email){
		email = *arguments.input->email;
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
