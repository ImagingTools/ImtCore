// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantMembership.h>
#include <imtauth/ITenantRole.h>


namespace imtauth
{


/**
	Interface for managing tenant memberships.
	Provides CRUD operations for the User ↔ Tenant relationship.

	Analogous to GitHub's Organization Membership management API.
	\ingroup TenantMembership
*/
class ITenantMembershipManager: virtual public istd::IChangeable
{
public:
	typedef QByteArrayList MembershipIds;

	/**
		Get all membership IDs for a specific tenant.
	*/
	virtual MembershipIds GetMembershipsByTenant(const QByteArray& tenantId) const = 0;

	/**
		Get all membership IDs for a specific user (across all tenants).
	*/
	virtual MembershipIds GetMembershipsByUser(const QByteArray& userId) const = 0;

	/**
		Get a specific membership by ID.
		\return UniquePtr to the membership object, or invalid ptr if not found.
	*/
	virtual ITenantMembershipUniquePtr GetMembership(const QByteArray& membershipId) const = 0;

	/**
		Find membership for a specific user in a specific tenant.
		\return UniquePtr to the membership, or invalid ptr if the user is not a member of the tenant.
	*/
	virtual ITenantMembershipUniquePtr FindMembership(const QByteArray& userId, const QByteArray& tenantId) const = 0;

	/**
		Add a user to a tenant with the specified role.
		Creates a new membership entity.
		\return The ID of the newly created membership, or empty on failure.
	*/
	virtual QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role) = 0;

	/**
		Invite a user to a tenant. The created membership remains inactive until the user accepts it.
	*/
	virtual QByteArray InviteMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role) = 0;

	/**
		Remove a membership (remove user from tenant).
		\return true if removed successfully.
	*/
	virtual bool RemoveMembership(const QByteArray& membershipId) = 0;

	/**
		Update the role of an existing membership.
		\return true if updated successfully.
	*/
	virtual bool UpdateMembershipRole(const QByteArray& membershipId, ITenantMembership::TenantMemberRole newRole) = 0;

	/**
		Accept a pending membership invitation.
	*/
	virtual bool AcceptMembershipInvitation(const QByteArray& membershipId) = 0;

	/**
		Reject a pending membership invitation.
	*/
	virtual bool RejectMembershipInvitation(const QByteArray& membershipId) = 0;

	/**
		Check if a user is a member of a specific tenant.
	*/
	virtual bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const = 0;

	/**
		Check if a user has at least the specified role in a tenant.
		Role hierarchy: Owner > Admin > Member > Guest.
	*/
	virtual bool HasMinimumRole(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole minimumRole) const = 0;

	// --- Tenant-scoped role management ---

	/**
		Get all tenant-scoped role IDs for a specific tenant.
	*/
	virtual QByteArrayList GetTenantRoleIds(const QByteArray& tenantId) const = 0;

	/**
		Get a tenant-scoped role by its ID.
	*/
	virtual ITenantRoleUniquePtr GetTenantRole(const QByteArray& roleId) const = 0;

	/**
		Create a new tenant-scoped role.
		\return The ID of the newly created role, or empty on failure.
	*/
	virtual QByteArray CreateTenantRole(const QByteArray& tenantId, const QString& roleName, const QString& permissions) = 0;

	/**
		Update an existing tenant-scoped role.
		\return true if updated successfully.
	*/
	virtual bool UpdateTenantRole(const QByteArray& roleId, const QString& roleName, const QString& permissions) = 0;

	/**
		Remove a tenant-scoped role.
		\return true if removed successfully.
	*/
	virtual bool RemoveTenantRole(const QByteArray& roleId) = 0;
};


} // namespace imtauth
