// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QByteArray>
#include <QByteArrayList>


namespace imtauth
{


/**
	Defines the access level for a resource.
	Analogous to GitHub's repository permission levels.
	Levels are ordered: None < Read < Write < Maintain < Admin.
*/
enum ResourceAccessLevel
{
	/**
		No access to the resource.
	*/
	RAL_None = 0,

	/**
		Read-only access (view, clone).
		Analogous to GitHub "Read" permission.
	*/
	RAL_Read,

	/**
		Read and write access (edit, create).
		Analogous to GitHub "Write" / "Triage" permission.
	*/
	RAL_Write,

	/**
		Extended write access (manage settings, but not destructive).
		Analogous to GitHub "Maintain" permission.
	*/
	RAL_Maintain,

	/**
		Full access (delete, transfer, manage access).
		Analogous to GitHub "Admin" permission.
	*/
	RAL_Admin
};


/**
	Interface for resource-level permission checking.
	Provides the mechanism to check effective permissions for a user on a specific resource,
	taking into account tenant membership, team assignments, and direct grants.

	Permission resolution order (highest wins):
	1. Direct user grant on resource
	2. Team grant on resource (highest across all teams)
	3. Tenant-level default (based on membership role)

	Analogous to GitHub's repository-level permission model.
	\ingroup Permission
*/
class IResourcePermission: virtual public istd::IPolymorphic
{
public:
	/**
		Get the effective access level for a user on a specific resource.
		Resolves through: direct grant → team grants → tenant default.
		\param userId The user to check.
		\param resourceId The resource (e.g., collection, document) to check access to.
		\param tenantId The tenant context.
		\return The effective access level.
	*/
	virtual ResourceAccessLevel GetEffectiveAccess(
		const QByteArray& userId,
		const QByteArray& resourceId,
		const QByteArray& tenantId) const = 0;

	/**
		Check if a user has at least the specified access level on a resource.
		\param userId The user to check.
		\param resourceId The resource to check.
		\param tenantId The tenant context.
		\param requiredLevel The minimum access level required.
		\return true if the user meets or exceeds the required level.
	*/
	virtual bool HasAccess(
		const QByteArray& userId,
		const QByteArray& resourceId,
		const QByteArray& tenantId,
		ResourceAccessLevel requiredLevel) const = 0;

	/**
		Grant a specific access level to a user on a resource (direct grant).
		\return true if granted successfully.
	*/
	virtual bool GrantAccess(
		const QByteArray& userId,
		const QByteArray& resourceId,
		const QByteArray& tenantId,
		ResourceAccessLevel level) = 0;

	/**
		Revoke direct access grant for a user on a resource.
		Does not affect team-inherited or tenant-default access.
		\return true if revoked successfully.
	*/
	virtual bool RevokeAccess(
		const QByteArray& userId,
		const QByteArray& resourceId,
		const QByteArray& tenantId) = 0;

	/**
		Grant access to a team (UserGroup) on a resource.
		All team members inherit this access level.
		\return true if granted successfully.
	*/
	virtual bool GrantTeamAccess(
		const QByteArray& teamId,
		const QByteArray& resourceId,
		const QByteArray& tenantId,
		ResourceAccessLevel level) = 0;

	/**
		Revoke team access on a resource.
		\return true if revoked successfully.
	*/
	virtual bool RevokeTeamAccess(
		const QByteArray& teamId,
		const QByteArray& resourceId,
		const QByteArray& tenantId) = 0;

	/**
		Get all user IDs that have direct or team-inherited access to a resource.
	*/
	virtual QByteArrayList GetResourceMembers(
		const QByteArray& resourceId,
		const QByteArray& tenantId) const = 0;
};


} // namespace imtauth
