// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// Leaves of the permission tree of an organization (\see imtauth::COrganizationsPermissionsProviderComp).
static const inline QByteArray s_changeOrganizationNamePermission = QByteArrayLiteral("/OrganizationManagement/Edit/ChangeName");
static const inline QByteArray s_changeOrganizationDescriptionPermission = QByteArrayLiteral("/OrganizationManagement/Edit/ChangeDescription");
static const inline QByteArray s_inviteOrganizationMemberPermission = QByteArrayLiteral("/OrganizationManagement/Members/Edit/Invite");
static const inline QByteArray s_excludeOrganizationMemberPermission = QByteArrayLiteral("/OrganizationManagement/Members/Edit/Exclude");
static const inline QByteArray s_changeOrganizationMemberRolePermission = QByteArrayLiteral("/OrganizationManagement/Members/Edit/ChangeRole");
static const inline QByteArray s_viewOrganizationMembersPermission = QByteArrayLiteral("/OrganizationManagement/Members/View");
static const inline QByteArray s_viewOrganizationRolesPermission = QByteArrayLiteral("/OrganizationManagement/Roles/View");
static const inline QByteArray s_changeOrganizationRolePermission = QByteArrayLiteral("/OrganizationManagement/Roles/Edit/Change");
static const inline QByteArray s_viewOrganizationGroupsPermission = QByteArrayLiteral("/OrganizationManagement/Groups/View");
static const inline QByteArray s_changeOrganizationGroupPermission = QByteArrayLiteral("/OrganizationManagement/Groups/Edit/Change");
static const inline QByteArray s_viewOrganizationPermissionsPermission = QByteArrayLiteral("/OrganizationManagement/Permissions/View");
static const inline QByteArray s_viewOrganizationConnectionsPermission = QByteArrayLiteral("/OrganizationManagement/Connections/View");
static const inline QByteArray s_viewOrganizationConnectionCodePermission = QByteArrayLiteral("/OrganizationManagement/Connections/ViewCode");
static const inline QByteArray s_connectOrganizationPermission = QByteArrayLiteral("/OrganizationManagement/Connections/Edit/Connect");
static const inline QByteArray s_removeOrganizationConnectionPermission = QByteArrayLiteral("/OrganizationManagement/Connections/Edit/Remove");

/**
	Every leaf that edits the organization itself, or its members.
*/
QByteArrayList GetEditOrganizationPermissions();
QByteArrayList GetEditOrganizationMemberPermissions();

/**
	Tell whether the organization permissions held include any of the requested ones (\see imtauth::HasPermission).
*/
bool HasAnyOrganizationPermission(const QByteArrayList& heldPermissions, const QByteArrayList& requestedPermissions);


/**
	Access to the data of the organization: system administrators, and otherwise
	the organization of the request context, or (without a context) its owner and members.
*/
bool HasTenantIsolationAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			QString& errorMessage);

/**
	Management of the organization: system administrators, its owner and creator,
	and members holding any of \c requiredOrgPermissions; never from the context of another organization.
*/
bool HasTenantManageAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			const QByteArrayList& requiredOrgPermissions,
			QString& errorMessage);


} // namespace imtauthgql


