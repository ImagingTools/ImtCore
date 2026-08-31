// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/COrganizationsPermissionsProviderComp.h>


// Qt includes
#include <QtCore/qglobal.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>

// ACF includes
#include <istd/TDelPtr.h>


namespace imtauth
{


// public methods

imtlic::IFeatureInfoSharedPtr COrganizationsPermissionsProviderComp::GetFeatureInfo(const QByteArray& featureId) const
{
	imtlic::IFeatureInfoSharedPtr direct = m_featureCollection.GetFeatureInfo(featureId);
	if (direct.IsValid()){
		return direct;
	}

	return m_featureCollection.FindFeatureById(featureId);
}


const imtbase::ICollectionInfo& COrganizationsPermissionsProviderComp::GetFeatureList() const
{
	return m_featureCollection.GetFeatureList();
}


// protected methods

void COrganizationsPermissionsProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	CreateOrganizationFeatures();
}


// private methods

void COrganizationsPermissionsProviderComp::CreateOrganizationFeatures()
{
	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationFeatureInfo;
	editOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationFeatureInfo->SetFeatureId("EditOrganization");
	editOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization"));
	editOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization."));
	editOrganizationFeatureInfo->SetOptional(false);
	editOrganizationFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationNameFeatureInfo;
	changeOrganizationNameFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationNameFeatureInfo->SetFeatureId("ChangeOrganizationName");
	changeOrganizationNameFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Name"));
	changeOrganizationNameFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Name."));
	changeOrganizationNameFeatureInfo->SetOptional(false);
	changeOrganizationNameFeatureInfo->SetIsPermission(true);
	editOrganizationFeatureInfo->InsertSubFeature(changeOrganizationNameFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationDescriptionFeatureInfo;
	changeOrganizationDescriptionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationDescriptionFeatureInfo->SetFeatureId("ChangeOrganizationDescription");
	changeOrganizationDescriptionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Description"));
	changeOrganizationDescriptionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Description."));
	changeOrganizationDescriptionFeatureInfo->SetOptional(false);
	changeOrganizationDescriptionFeatureInfo->SetIsPermission(true);
	editOrganizationFeatureInfo->InsertSubFeature(changeOrganizationDescriptionFeatureInfo.PopPtr());

	m_featureCollection.InsertNewObject("FeatureInfo", "", "", editOrganizationFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> memberManagementFeatureInfo;
	memberManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	memberManagementFeatureInfo->SetFeatureId("MemberManagement");
	memberManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Member Management"));
	memberManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Member-related operations."));
	memberManagementFeatureInfo->SetOptional(false);
	memberManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationMembersFeatureInfo;
	viewOrganizationMembersFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationMembersFeatureInfo->SetFeatureId("ViewOrganizationMembers");
	viewOrganizationMembersFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Members"));
	viewOrganizationMembersFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Members."));
	viewOrganizationMembersFeatureInfo->SetOptional(false);
	viewOrganizationMembersFeatureInfo->SetIsPermission(true);
	viewOrganizationMembersFeatureInfo->SetRequirements(QByteArray("EditOrganizationMember;InviteOrganizationMember;ExcludeOrganizationMember;ChangeOrganizationMember;ChangeOrganizationMemberRole").split(';'));
	memberManagementFeatureInfo->InsertSubFeature(viewOrganizationMembersFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationMemberFeatureInfo;
	editOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationMemberFeatureInfo->SetFeatureId("EditOrganizationMember");
	editOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Member"));
	editOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Member."));
	editOrganizationMemberFeatureInfo->SetOptional(false);
	editOrganizationMemberFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> inviteOrganizationMemberFeatureInfo;
	inviteOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	inviteOrganizationMemberFeatureInfo->SetFeatureId("InviteOrganizationMember");
	inviteOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Invite Organization Member"));
	inviteOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows inviting Organization Member."));
	inviteOrganizationMemberFeatureInfo->SetOptional(false);
	inviteOrganizationMemberFeatureInfo->SetIsPermission(true);
	editOrganizationMemberFeatureInfo->InsertSubFeature(inviteOrganizationMemberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> excludeOrganizationMemberFeatureInfo;
	excludeOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	excludeOrganizationMemberFeatureInfo->SetFeatureId("ExcludeOrganizationMember");
	excludeOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Exclude Organization Member"));
	excludeOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows excluding a member from the organization."));
	excludeOrganizationMemberFeatureInfo->SetOptional(false);
	excludeOrganizationMemberFeatureInfo->SetIsPermission(true);
	editOrganizationMemberFeatureInfo->InsertSubFeature(excludeOrganizationMemberFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationMemberFeatureInfo;
	changeOrganizationMemberFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationMemberFeatureInfo->SetFeatureId("ChangeOrganizationMember");
	changeOrganizationMemberFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Member"));
	changeOrganizationMemberFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing a member within the organization."));
	changeOrganizationMemberFeatureInfo->SetOptional(false);
	changeOrganizationMemberFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationMemberRoleFeatureInfo;
	changeOrganizationMemberRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationMemberRoleFeatureInfo->SetFeatureId("ChangeOrganizationMemberRole");
	changeOrganizationMemberRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Member Role"));
	changeOrganizationMemberRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Member Role."));
	changeOrganizationMemberRoleFeatureInfo->SetOptional(false);
	changeOrganizationMemberRoleFeatureInfo->SetIsPermission(true);
	changeOrganizationMemberFeatureInfo->InsertSubFeature(changeOrganizationMemberRoleFeatureInfo.PopPtr());
	editOrganizationMemberFeatureInfo->InsertSubFeature(changeOrganizationMemberFeatureInfo.PopPtr());

	memberManagementFeatureInfo->InsertSubFeature(editOrganizationMemberFeatureInfo.PopPtr());
	m_featureCollection.InsertNewObject("FeatureInfo", "", "", memberManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> roleManagementFeatureInfo;
	roleManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	roleManagementFeatureInfo->SetFeatureId("RoleManagement");
	roleManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Role Management"));
	roleManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Role-related operations."));
	roleManagementFeatureInfo->SetOptional(false);
	roleManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationRolesFeatureInfo;
	viewOrganizationRolesFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationRolesFeatureInfo->SetFeatureId("ViewOrganizationRoles");
	viewOrganizationRolesFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Roles"));
	viewOrganizationRolesFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Roles."));
	viewOrganizationRolesFeatureInfo->SetOptional(false);
	viewOrganizationRolesFeatureInfo->SetIsPermission(true);
	viewOrganizationRolesFeatureInfo->SetRequirements(QByteArray("EditOrganizationRole;ChangeOrganizationRole;RemoveOrganizationRole;AddOrganizationRole").split(';'));
	roleManagementFeatureInfo->InsertSubFeature(viewOrganizationRolesFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationRoleFeatureInfo;
	editOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationRoleFeatureInfo->SetFeatureId("EditOrganizationRole");
	editOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Role"));
	editOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Role."));
	editOrganizationRoleFeatureInfo->SetOptional(false);
	editOrganizationRoleFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationRoleFeatureInfo;
	changeOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationRoleFeatureInfo->SetFeatureId("ChangeOrganizationRole");
	changeOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Role"));
	changeOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Role."));
	changeOrganizationRoleFeatureInfo->SetOptional(false);
	changeOrganizationRoleFeatureInfo->SetIsPermission(true);
	editOrganizationRoleFeatureInfo->InsertSubFeature(changeOrganizationRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationRoleFeatureInfo;
	removeOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationRoleFeatureInfo->SetFeatureId("RemoveOrganizationRole");
	removeOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Role"));
	removeOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Role."));
	removeOrganizationRoleFeatureInfo->SetOptional(false);
	removeOrganizationRoleFeatureInfo->SetIsPermission(true);
	editOrganizationRoleFeatureInfo->InsertSubFeature(removeOrganizationRoleFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrganizationRoleFeatureInfo;
	addOrganizationRoleFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrganizationRoleFeatureInfo->SetFeatureId("AddOrganizationRole");
	addOrganizationRoleFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization Role"));
	addOrganizationRoleFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Organization Role."));
	addOrganizationRoleFeatureInfo->SetOptional(false);
	addOrganizationRoleFeatureInfo->SetIsPermission(true);
	editOrganizationRoleFeatureInfo->InsertSubFeature(addOrganizationRoleFeatureInfo.PopPtr());

	roleManagementFeatureInfo->InsertSubFeature(editOrganizationRoleFeatureInfo.PopPtr());
	m_featureCollection.InsertNewObject("FeatureInfo", "", "", roleManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> groupManagementFeatureInfo;
	groupManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	groupManagementFeatureInfo->SetFeatureId("GroupManagement");
	groupManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Group Management"));
	groupManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Group-related operations."));
	groupManagementFeatureInfo->SetOptional(false);
	groupManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationGroupsFeatureInfo;
	viewOrganizationGroupsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationGroupsFeatureInfo->SetFeatureId("ViewOrganizationGroups");
	viewOrganizationGroupsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Groups"));
	viewOrganizationGroupsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Groups."));
	viewOrganizationGroupsFeatureInfo->SetOptional(false);
	viewOrganizationGroupsFeatureInfo->SetIsPermission(true);
	viewOrganizationGroupsFeatureInfo->SetRequirements(QByteArray("EditOrganizationGroup;ChangeOrganizationGroup;RemoveOrganizationGroup;AddOrganizationGroup").split(';'));
	groupManagementFeatureInfo->InsertSubFeature(viewOrganizationGroupsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationGroupFeatureInfo;
	editOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationGroupFeatureInfo->SetFeatureId("EditOrganizationGroup");
	editOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Group"));
	editOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Group."));
	editOrganizationGroupFeatureInfo->SetOptional(false);
	editOrganizationGroupFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> changeOrganizationGroupFeatureInfo;
	changeOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	changeOrganizationGroupFeatureInfo->SetFeatureId("ChangeOrganizationGroup");
	changeOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Change Organization Group"));
	changeOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows changing Organization Group."));
	changeOrganizationGroupFeatureInfo->SetOptional(false);
	changeOrganizationGroupFeatureInfo->SetIsPermission(true);
	editOrganizationGroupFeatureInfo->InsertSubFeature(changeOrganizationGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationGroupFeatureInfo;
	removeOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationGroupFeatureInfo->SetFeatureId("RemoveOrganizationGroup");
	removeOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Group"));
	removeOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Group."));
	removeOrganizationGroupFeatureInfo->SetOptional(false);
	removeOrganizationGroupFeatureInfo->SetIsPermission(true);
	editOrganizationGroupFeatureInfo->InsertSubFeature(removeOrganizationGroupFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> addOrganizationGroupFeatureInfo;
	addOrganizationGroupFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	addOrganizationGroupFeatureInfo->SetFeatureId("AddOrganizationGroup");
	addOrganizationGroupFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Add Organization Group"));
	addOrganizationGroupFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows adding Organization Group."));
	addOrganizationGroupFeatureInfo->SetOptional(false);
	addOrganizationGroupFeatureInfo->SetIsPermission(true);
	editOrganizationGroupFeatureInfo->InsertSubFeature(addOrganizationGroupFeatureInfo.PopPtr());

	groupManagementFeatureInfo->InsertSubFeature(editOrganizationGroupFeatureInfo.PopPtr());
	m_featureCollection.InsertNewObject("FeatureInfo", "", "", groupManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> permissionManagementFeatureInfo;
	permissionManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	permissionManagementFeatureInfo->SetFeatureId("PermissionManagement");
	permissionManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Permission Management"));
	permissionManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Permission-related operations."));
	permissionManagementFeatureInfo->SetOptional(false);
	permissionManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationPermissionsFeatureInfo;
	viewOrganizationPermissionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationPermissionsFeatureInfo->SetFeatureId("ViewOrganizationPermissions");
	viewOrganizationPermissionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Permissions"));
	viewOrganizationPermissionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Permissions."));
	viewOrganizationPermissionsFeatureInfo->SetOptional(false);
	viewOrganizationPermissionsFeatureInfo->SetIsPermission(true);
	permissionManagementFeatureInfo->InsertSubFeature(viewOrganizationPermissionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationMemberPermissionsFeatureInfo;
	editOrganizationMemberPermissionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationMemberPermissionsFeatureInfo->SetFeatureId("EditOrganizationMemberPermissions");
	editOrganizationMemberPermissionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Member Permissions"));
	editOrganizationMemberPermissionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows assigning and modifying permissions for organization members."));
	editOrganizationMemberPermissionsFeatureInfo->SetOptional(false);
	editOrganizationMemberPermissionsFeatureInfo->SetIsPermission(true);
	permissionManagementFeatureInfo->InsertSubFeature(editOrganizationMemberPermissionsFeatureInfo.PopPtr());

	m_featureCollection.InsertNewObject("FeatureInfo", "", "", permissionManagementFeatureInfo.GetPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> connectionManagementFeatureInfo;
	connectionManagementFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	connectionManagementFeatureInfo->SetFeatureId("ConnectionManagement");
	connectionManagementFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Connection Management"));
	connectionManagementFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Provides access to Connection-related operations."));
	connectionManagementFeatureInfo->SetOptional(false);
	connectionManagementFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationConnectionsFeatureInfo;
	viewOrganizationConnectionsFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationConnectionsFeatureInfo->SetFeatureId("ViewOrganizationConnections");
	viewOrganizationConnectionsFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Connections"));
	viewOrganizationConnectionsFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Connections."));
	viewOrganizationConnectionsFeatureInfo->SetOptional(false);
	viewOrganizationConnectionsFeatureInfo->SetIsPermission(true);
	viewOrganizationConnectionsFeatureInfo->SetRequirements(QByteArray("EditOrganizationConnection;ConnectOrganization;RemoveOrganizationConnection").split(';'));
	connectionManagementFeatureInfo->InsertSubFeature(viewOrganizationConnectionsFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> editOrganizationConnectionFeatureInfo;
	editOrganizationConnectionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	editOrganizationConnectionFeatureInfo->SetFeatureId("EditOrganizationConnection");
	editOrganizationConnectionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Edit Organization Connection"));
	editOrganizationConnectionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows editing Organization Connection."));
	editOrganizationConnectionFeatureInfo->SetOptional(false);
	editOrganizationConnectionFeatureInfo->SetIsPermission(true);

	istd::TDelPtr<imtlic::CFeatureInfo> connectOrganizationFeatureInfo;
	connectOrganizationFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	connectOrganizationFeatureInfo->SetFeatureId("ConnectOrganization");
	connectOrganizationFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Connect Organization"));
	connectOrganizationFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows connecting Organization."));
	connectOrganizationFeatureInfo->SetOptional(false);
	connectOrganizationFeatureInfo->SetIsPermission(true);
	editOrganizationConnectionFeatureInfo->InsertSubFeature(connectOrganizationFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> removeOrganizationConnectionFeatureInfo;
	removeOrganizationConnectionFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	removeOrganizationConnectionFeatureInfo->SetFeatureId("RemoveOrganizationConnection");
	removeOrganizationConnectionFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "Remove Organization Connection"));
	removeOrganizationConnectionFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows removing Organization Connection."));
	removeOrganizationConnectionFeatureInfo->SetOptional(false);
	removeOrganizationConnectionFeatureInfo->SetIsPermission(true);
	editOrganizationConnectionFeatureInfo->InsertSubFeature(removeOrganizationConnectionFeatureInfo.PopPtr());

	connectionManagementFeatureInfo->InsertSubFeature(editOrganizationConnectionFeatureInfo.PopPtr());

	istd::TDelPtr<imtlic::CFeatureInfo> viewOrganizationConnectionCodeFeatureInfo;
	viewOrganizationConnectionCodeFeatureInfo.SetPtr(new imtlic::CFeatureInfo);
	viewOrganizationConnectionCodeFeatureInfo->SetFeatureId("ViewOrganizationConnectionCode");
	viewOrganizationConnectionCodeFeatureInfo->SetFeatureName(QT_TRANSLATE_NOOP("Feature", "View Organization Connection Code"));
	viewOrganizationConnectionCodeFeatureInfo->SetFeatureDescription(QT_TRANSLATE_NOOP("Feature", "Allows viewing Organization Connection Code."));
	viewOrganizationConnectionCodeFeatureInfo->SetOptional(false);
	viewOrganizationConnectionCodeFeatureInfo->SetIsPermission(true);
	connectionManagementFeatureInfo->InsertSubFeature(viewOrganizationConnectionCodeFeatureInfo.PopPtr());
	m_featureCollection.InsertNewObject("FeatureInfo", "", "", connectionManagementFeatureInfo.GetPtr());
}


} // namespace imtauth
