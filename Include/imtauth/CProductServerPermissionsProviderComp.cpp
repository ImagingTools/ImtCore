// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CProductServerPermissionsProviderComp.h>


// Qt includes
#include <QtCore/qglobal.h>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>


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

imtlic::IFeatureInfoSharedPtr CProductServerPermissionsProviderComp::GetFeatureInfo(const QByteArray& featureId) const
{
	imtlic::IFeatureInfoSharedPtr direct = m_featureCollection.GetFeatureInfo(featureId);
	if (direct.IsValid()){
		return direct;
	}

	return m_featureCollection.FindFeatureById(featureId);
}


const imtbase::ICollectionInfo& CProductServerPermissionsProviderComp::GetFeatureList() const
{
	return m_featureCollection.GetFeatureList();
}


// protected methods

void CProductServerPermissionsProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	CreateFeatures();
}


// private methods

void CProductServerPermissionsProviderComp::CreateFeatures()
{
	istd::TDelPtr<imtlic::CFeatureInfo> serverPtr(CreatePermission(
				"ProductServer",
				QT_TRANSLATE_NOOP("Feature", "Product Server"),
				QT_TRANSLATE_NOOP("Feature", "What the server of the product does at this server on its own behalf.")));

	AddPermission(*serverPtr, "SignIn",
				QT_TRANSLATE_NOOP("Feature", "Sign-in"),
				QT_TRANSLATE_NOOP("Feature", "Signs the users of the product in: sessions, profiles, passwords, and reading the users, roles, groups and organizations their permissions come from."));
	AddPermission(*serverPtr, "PublishPermissions",
				QT_TRANSLATE_NOOP("Feature", "Publish Permissions"),
				QT_TRANSLATE_NOOP("Feature", "Publishes the permission tree of the product."));

	// The administration pages of the product reach the users, roles and groups of the product here
	imtlic::CFeatureInfo* usersPtr = AddPermission(*serverPtr, "Users",
				QT_TRANSLATE_NOOP("Feature", "Users"),
				QT_TRANSLATE_NOOP("Feature", "Users of the product."));
	AddPermission(*usersPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Users"),
				QT_TRANSLATE_NOOP("Feature", "Reads users, their sessions and their changes."));
	AddPermission(*usersPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Users"),
				QT_TRANSLATE_NOOP("Feature", "Administers users on the pages of the product."),
				{QByteArrayLiteral("/ProductServer/Users/View"), QByteArrayLiteral("/ProductServer/Roles/View"), QByteArrayLiteral("/ProductServer/Groups/View")});

	imtlic::CFeatureInfo* rolesPtr = AddPermission(*serverPtr, "Roles",
				QT_TRANSLATE_NOOP("Feature", "Roles"),
				QT_TRANSLATE_NOOP("Feature", "Roles of the product."));
	AddPermission(*rolesPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Roles"),
				QT_TRANSLATE_NOOP("Feature", "Reads roles and their changes."));
	AddPermission(*rolesPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Roles"),
				QT_TRANSLATE_NOOP("Feature", "Administers roles on the pages of the product."),
				{QByteArrayLiteral("/ProductServer/Roles/View")});

	imtlic::CFeatureInfo* groupsPtr = AddPermission(*serverPtr, "Groups",
				QT_TRANSLATE_NOOP("Feature", "Groups"),
				QT_TRANSLATE_NOOP("Feature", "Groups of the product."));
	AddPermission(*groupsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Groups"),
				QT_TRANSLATE_NOOP("Feature", "Reads groups and their changes."));
	AddPermission(*groupsPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Groups"),
				QT_TRANSLATE_NOOP("Feature", "Administers groups on the pages of the product."),
				{QByteArrayLiteral("/ProductServer/Groups/View"), QByteArrayLiteral("/ProductServer/Users/View"), QByteArrayLiteral("/ProductServer/Roles/View")});

	imtlic::CFeatureInfo* tokensPtr = AddPermission(*serverPtr, "PersonalTokens",
				QT_TRANSLATE_NOOP("Feature", "Personal Tokens"),
				QT_TRANSLATE_NOOP("Feature", "Personal access tokens of the users of the product."));
	AddPermission(*tokensPtr, "Accept",
				QT_TRANSLATE_NOOP("Feature", "Accept Personal Tokens"),
				QT_TRANSLATE_NOOP("Feature", "Checks the personal access tokens issued for the product."));
	AddPermission(*tokensPtr, "Manage",
				QT_TRANSLATE_NOOP("Feature", "Manage Personal Tokens"),
				QT_TRANSLATE_NOOP("Feature", "Lets the users manage their personal access tokens in the product."));

	imtlic::CFeatureInfo* organizationsPtr = AddPermission(*serverPtr, "Organizations",
				QT_TRANSLATE_NOOP("Feature", "Organizations"),
				QT_TRANSLATE_NOOP("Feature", "Organizations of the users of the product."));
	AddPermission(*organizationsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "View Organizations"),
				QT_TRANSLATE_NOOP("Feature", "Reads organizations, members, relationships and invitations."));
	AddPermission(*organizationsPtr, "Edit",
				QT_TRANSLATE_NOOP("Feature", "Edit Organizations"),
				QT_TRANSLATE_NOOP("Feature", "Administers organizations on the pages of the product."),
				{QByteArrayLiteral("/ProductServer/Organizations/View")});

	imtlic::CFeatureInfo* notificationsPtr = AddPermission(*serverPtr, "Notifications",
				QT_TRANSLATE_NOOP("Feature", "Notifications"),
				QT_TRANSLATE_NOOP("Feature", "Notifications of the users of the product."));
	AddPermission(*notificationsPtr, "View",
				QT_TRANSLATE_NOOP("Feature", "Notification Center"),
				QT_TRANSLATE_NOOP("Feature", "Lets the users read their notifications in the product."));
	AddPermission(*notificationsPtr, "Send",
				QT_TRANSLATE_NOOP("Feature", "Send Notifications"),
				QT_TRANSLATE_NOOP("Feature", "Sends notifications to the users of the product."));

	m_featureCollection.InsertNewObject("FeatureInfo", "", "", serverPtr.GetPtr());
}


} // namespace imtauth


