// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantMembership.h>


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
		\return Pointer to the membership object, or nullptr if not found.
	*/
	virtual const ITenantMembership* GetMembership(const QByteArray& membershipId) const = 0;

	/**
		Find membership for a specific user in a specific tenant.
		\return Pointer to the membership, or nullptr if the user is not a member of the tenant.
	*/
	virtual const ITenantMembership* FindMembership(const QByteArray& userId, const QByteArray& tenantId) const = 0;

	/**
		Add a user to a tenant with the specified role.
		Creates a new membership entity.
		\return The ID of the newly created membership, or empty on failure.
	*/
	virtual QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role) = 0;

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
		Check if a user is a member of a specific tenant.
	*/
	virtual bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const = 0;

	/**
		Check if a user has at least the specified role in a tenant.
		Role hierarchy: Owner > Admin > Member > Guest.
	*/
	virtual bool HasMinimumRole(const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole minimumRole) const = 0;
};


} // namespace imtauth
