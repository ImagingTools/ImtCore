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


namespace
{


imtlic::CFeatureInfo* CreatePermission(
			const QByteArray& featureId,
			const char* featureName,
			const char* featureDescription,
			const QByteArrayList& requirements = QByteArrayList())
{
	imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo;
	featureInfoPtr->SetFeatureId(featureId);
	featureInfoPtr->SetFeatureName(featureName);
	featureInfoPtr->SetFeatureDescription(featureDescription);
	featureInfoPtr->SetOptional(false);
	featureInfoPtr->SetIsPermission(true);
	featureInfoPtr->SetRequirements(requirements);

	return featureInfoPtr;
}


imtlic::CFeatureInfo* AddPermission(
			imtlic::CFeatureInfo& parent,
			const QByteArray& featureId,
			const char* featureName,
			const char* featureDescription,
			const QByteArrayList& requirements = QByteArrayList())
{
	imtlic::CFeatureInfo* featureInfoPtr = CreatePermission(featureId, featureName, featureDescription, requirements);
	parent.InsertSubFeature(imtlic::IFeatureInfo::FeatureInfoPtr(featureInfoPtr));

	return featureInfoPtr;
}


} // anonymous namespace


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
	istd::TDelPtr<imtlic::CFeatureInfo> organizationPtr(CreatePermission(
				"OrganizationManagement",
				QT_TRANSLATE_NOOP("Feature", "Organization Management"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the operations inside an organization.")));

	imtlic::CFeatureInfo* editPtr = AddPermission(*organizationPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Organization"),
				QT_TRANSLATE_NOOP("Feature", "Allows editing the organization."));
	AddPermission(*editPtr, "ChangeName",
				QT_TRANSLATE_NOOP("Feature", "Change Name"),
				QT_TRANSLATE_NOOP("Feature", "Allows changing the name of the organization."));
	AddPermission(*editPtr, "ChangeDescription",
				QT_TRANSLATE_NOOP("Feature", "Change Description"),
				QT_TRANSLATE_NOOP("Feature", "Allows changing the description of the organization."));

	imtlic::CFeatureInfo* membersPtr = AddPermission(*organizationPtr, "Members",
				QT_TRANSLATE_NOOP("Feature", "Members"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the members of the organization."));
	AddPermission(*membersPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Members"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the members of the organization."));
	imtlic::CFeatureInfo* editMembersPtr = AddPermission(*membersPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Members"),
				QT_TRANSLATE_NOOP("Feature", "Allows editing the members of the organization."),
				{QByteArrayLiteral("/OrganizationManagement/Members/View")});
	AddPermission(*editMembersPtr, "Add",
				QT_TRANSLATE_NOOP("Feature", "Create Member"),
				QT_TRANSLATE_NOOP("Feature", "Allows creating a user who becomes a member of the organization."));
	AddPermission(*editMembersPtr, "Change",
				QT_TRANSLATE_NOOP("Feature", "Change Member"),
				QT_TRANSLATE_NOOP("Feature", "Allows changing the account data of a member."));
	AddPermission(*editMembersPtr, "Invite",
				QT_TRANSLATE_NOOP("Feature", "Invite Member"),
				QT_TRANSLATE_NOOP("Feature", "Allows inviting a user to the organization."));
	AddPermission(*editMembersPtr, "Exclude",
				QT_TRANSLATE_NOOP("Feature", "Exclude Member"),
				QT_TRANSLATE_NOOP("Feature", "Allows excluding a member from the organization."));
	AddPermission(*editMembersPtr, "ChangeRole",
				QT_TRANSLATE_NOOP("Feature", "Change Member Role"),
				QT_TRANSLATE_NOOP("Feature", "Allows changing the role of a member."));

	imtlic::CFeatureInfo* rolesPtr = AddPermission(*organizationPtr, "Roles",
				QT_TRANSLATE_NOOP("Feature", "Roles"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the roles of the organization."));
	AddPermission(*rolesPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Roles"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the roles of the organization."));
	imtlic::CFeatureInfo* editRolesPtr = AddPermission(*rolesPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Roles"),
				QT_TRANSLATE_NOOP("Feature", "Allows editing the roles of the organization."),
				{QByteArrayLiteral("/OrganizationManagement/Roles/View")});
	AddPermission(*editRolesPtr, "Add",
				QT_TRANSLATE_NOOP("Feature", "Add Role"),
				QT_TRANSLATE_NOOP("Feature", "Allows adding a role."));
	AddPermission(*editRolesPtr, "Change",
				QT_TRANSLATE_NOOP("Feature", "Change Role"),
				QT_TRANSLATE_NOOP("Feature", "Allows changing a role."));
	AddPermission(*editRolesPtr, "Remove",
				QT_TRANSLATE_NOOP("Feature", "Remove Role"),
				QT_TRANSLATE_NOOP("Feature", "Allows removing a role."));

	imtlic::CFeatureInfo* groupsPtr = AddPermission(*organizationPtr, "Groups",
				QT_TRANSLATE_NOOP("Feature", "Groups"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the groups of the organization."));
	AddPermission(*groupsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Groups"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the groups of the organization."));
	imtlic::CFeatureInfo* editGroupsPtr = AddPermission(*groupsPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Groups"),
				QT_TRANSLATE_NOOP("Feature", "Allows editing the groups of the organization."),
				{QByteArrayLiteral("/OrganizationManagement/Groups/View")});
	AddPermission(*editGroupsPtr, "Add",
				QT_TRANSLATE_NOOP("Feature", "Add Group"),
				QT_TRANSLATE_NOOP("Feature", "Allows adding a group."));
	AddPermission(*editGroupsPtr, "Change",
				QT_TRANSLATE_NOOP("Feature", "Change Group"),
				QT_TRANSLATE_NOOP("Feature", "Allows changing a group."));
	AddPermission(*editGroupsPtr, "Remove",
				QT_TRANSLATE_NOOP("Feature", "Remove Group"),
				QT_TRANSLATE_NOOP("Feature", "Allows removing a group."));

	imtlic::CFeatureInfo* permissionsPtr = AddPermission(*organizationPtr, "Permissions",
				QT_TRANSLATE_NOOP("Feature", "Permissions"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the permissions of the members."));
	AddPermission(*permissionsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Permissions"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the permissions of the members."));
	AddPermission(*permissionsPtr, "Change",
				QT_TRANSLATE_NOOP("Feature", "Change Permissions"),
				QT_TRANSLATE_NOOP("Feature", "Allows assigning and changing the permissions of the members."),
				{QByteArrayLiteral("/OrganizationManagement/Permissions/View")});

	imtlic::CFeatureInfo* connectionsPtr = AddPermission(*organizationPtr, "Connections",
				QT_TRANSLATE_NOOP("Feature", "Connections"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the connections to other organizations."));
	AddPermission(*connectionsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Connections"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the connections to other organizations."));
	AddPermission(*connectionsPtr, "ViewCode",
				QT_TRANSLATE_NOOP("Feature", "View Connection Code"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the code other organizations connect with."));
	imtlic::CFeatureInfo* editConnectionsPtr = AddPermission(*connectionsPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Connections"),
				QT_TRANSLATE_NOOP("Feature", "Allows editing the connections to other organizations."),
				{QByteArrayLiteral("/OrganizationManagement/Connections/View")});
	AddPermission(*editConnectionsPtr, "Connect",
				QT_TRANSLATE_NOOP("Feature", "Connect Organization"),
				QT_TRANSLATE_NOOP("Feature", "Allows connecting to another organization."));
	AddPermission(*editConnectionsPtr, "Remove",
				QT_TRANSLATE_NOOP("Feature", "Remove Connection"),
				QT_TRANSLATE_NOOP("Feature", "Allows removing a connection to another organization."));

	imtlic::CFeatureInfo* contractsPtr = AddPermission(*organizationPtr, "Contracts",
				QT_TRANSLATE_NOOP("Feature", "Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the contracts with other organizations."));
	AddPermission(*contractsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Allows viewing the contracts, their attachments and journals."));
	AddPermission(*contractsPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Allows drafting and sending contracts, asking for changes and adding attachments."),
				{QByteArrayLiteral("/OrganizationManagement/Contracts/View")});
	AddPermission(*contractsPtr, "Approve",
				QT_TRANSLATE_NOOP("Feature", "Approve Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Allows approving the terms of a contract inside the organization."),
				{QByteArrayLiteral("/OrganizationManagement/Contracts/View")});
	AddPermission(*contractsPtr, "Sign",
				QT_TRANSLATE_NOOP("Feature", "Sign Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Allows signing and declining contracts, leaving and terminating them."),
				{QByteArrayLiteral("/OrganizationManagement/Contracts/View")});
	AddPermission(*contractsPtr, "Give",
				QT_TRANSLATE_NOOP("Feature", "Give Permissions"),
				QT_TRANSLATE_NOOP("Feature", "Allows giving the permissions of the organization to the other parties of its contracts and taking them back."),
				{QByteArrayLiteral("/OrganizationManagement/Contracts/View")});
	AddPermission(*contractsPtr, "Distribute",
				QT_TRANSLATE_NOOP("Feature", "Distribute Received Permissions"),
				QT_TRANSLATE_NOOP("Feature", "Allows giving the permissions received by a contract to the groups of the organization."),
				{QByteArrayLiteral("/OrganizationManagement/Contracts/View")});

	imtlic::CFeatureInfo* partyRolesPtr = AddPermission(*organizationPtr, "PartyRoles",
				QT_TRANSLATE_NOOP("Feature", "Roles of Parties"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the roles the organization plays in contracts."));
	AddPermission(*partyRolesPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Roles of Parties"),
				QT_TRANSLATE_NOOP("Feature", "Allows creating, changing and archiving the roles of parties of the organization."));

	imtlic::CFeatureInfo* contractTypesPtr = AddPermission(*organizationPtr, "ContractTypes",
				QT_TRANSLATE_NOOP("Feature", "Types of Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the templates of contracts of the organization."));
	AddPermission(*contractTypesPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Types of Contracts"),
				QT_TRANSLATE_NOOP("Feature", "Allows creating, changing and archiving the types of contracts of the organization."));

	imtlic::CFeatureInfo* directoryPtr = AddPermission(*organizationPtr, "Directory",
				QT_TRANSLATE_NOOP("Feature", "Directory"),
				QT_TRANSLATE_NOOP("Feature", "Provides access to the profile of the organization in the directory."));
	AddPermission(*directoryPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Profile in Directory"),
				QT_TRANSLATE_NOOP("Feature", "Allows publishing the organization in the directory and changing its profile."));

	m_featureCollection.InsertNewObject("FeatureInfo", "", "", organizationPtr.GetPtr());
}


} // namespace imtauth


