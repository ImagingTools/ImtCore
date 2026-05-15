// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QByteArray>


namespace imtauth
{


/**
	Tenant management permission IDs.
	These permissions control access to tenant management operations.
	The tenant Owner always bypasses permission checks.
	\ingroup TenantMembership
*/
namespace TenantPermissions
{
	/**
		Permission to invite new members to the tenant.
		Required for: CreateTenantInvitation
	*/
	inline const QByteArray InviteMembers()		{ return QByteArrayLiteral("tenant.invite"); }

	/**
		Permission to view tenant members and invitations.
		Required for: GetTenantInvitations, GetMembershipsByTenant
	*/
	inline const QByteArray ViewMembers()		{ return QByteArrayLiteral("tenant.view_members"); }

	/**
		Permission to manage existing members (remove, change roles).
		Required for: RemoveMembership, UpdateMembershipRole
	*/
	inline const QByteArray ManageMembers()		{ return QByteArrayLiteral("tenant.manage_members"); }

	/**
		Permission to manage invitations (revoke, resend).
		Required for: RevokeTenantInvitation, ResendTenantInvitation
	*/
	inline const QByteArray ManageInvitations()	{ return QByteArrayLiteral("tenant.manage_invitations"); }
}


} // namespace imtauth
