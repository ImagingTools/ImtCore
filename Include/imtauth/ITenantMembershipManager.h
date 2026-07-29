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
		\return UniquePtr to the membership object, or invalid ptr if not found.
	*/
	virtual ITenantMembershipUniquePtr GetMembership(const QByteArray& membershipId) const = 0;

	/**
		Find membership for a specific user in a specific tenant.
		\return UniquePtr to the membership, or invalid ptr if the user is not a member of the tenant.
	*/
	virtual ITenantMembershipUniquePtr FindMembership(const QByteArray& userId, const QByteArray& tenantId) const = 0;

	/**
		Add a user to a tenant.
		Creates a new membership entity.
		\return The ID of the newly created membership, or empty on failure.
	*/
	virtual QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId) = 0;

	/**
		Remove a membership (remove user from tenant).
		\return true if removed successfully.
	*/
	virtual bool RemoveMembership(const QByteArray& membershipId) = 0;

	/**
		Update the organization-specific permissions of an existing membership.
		These are the special permissions (from OrganizationsPermissionsProvider) granted to the member inside the org.
		\return true if updated successfully.
	*/
	virtual bool UpdateMembershipOrganizationPermissions(const QByteArray& membershipId, const QByteArrayList& permissions) = 0;

	/**
		Check if a user is a member of a specific tenant.
	*/
	virtual bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const = 0;

};


} // namespace imtauth
