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

	m_featureCollection.InsertNewObject("FeatureInfo", "", "", organizationPtr.GetPtr());
}


} // namespace imtauth


