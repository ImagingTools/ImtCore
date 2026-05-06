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
	Interface for managing team-level permissions within a tenant.
	Teams (UserGroups) can be scoped to a tenant and inherit/override permissions
	on resources.

	This builds on top of the existing IUserGroupInfo by adding tenant-scoping
	and permission management at the team level.

	Analogous to GitHub's Team permission model.
	\ingroup Permission
*/
class ITeamPermission: virtual public istd::IPolymorphic
{
public:
	/**
		Defines the role of a user within a team (UserGroup) inside a tenant.
		Analogous to GitHub's team membership roles.
	*/
	enum TeamMemberRole
	{
		TMR_MAINTAINER = 0,
		TMR_MEMBER
	};

	I_DECLARE_ENUM(TeamMemberRole, TMR_MAINTAINER, TMR_MEMBER);
	/**
		Get all teams (UserGroup IDs) within a tenant.
	*/
	virtual QByteArrayList GetTeamsByTenant(const QByteArray& tenantId) const = 0;

	/**
		Get all teams a user belongs to within a specific tenant.
	*/
	virtual QByteArrayList GetUserTeams(const QByteArray& userId, const QByteArray& tenantId) const = 0;

	/**
		Associate a team (UserGroup) with a tenant.
		After this, the team is scoped to the tenant for permission resolution.
		\return true if associated successfully.
	*/
	virtual bool AssociateTeamWithTenant(const QByteArray& teamId, const QByteArray& tenantId) = 0;

	/**
		Disassociate a team from a tenant.
		\return true if removed successfully.
	*/
	virtual bool DisassociateTeamFromTenant(const QByteArray& teamId, const QByteArray& tenantId) = 0;

	/**
		Get the role of a user within a team.
		\return The team member role, or TMR_MEMBER as default.
	*/
	virtual TeamMemberRole GetTeamMemberRole(const QByteArray& userId, const QByteArray& teamId) const = 0;

	/**
		Set the role of a user within a team.
		\return true if set successfully.
	*/
	virtual bool SetTeamMemberRole(const QByteArray& userId, const QByteArray& teamId, TeamMemberRole role) = 0;

	/**
		Check if a team is associated with a tenant.
	*/
	virtual bool IsTeamInTenant(const QByteArray& teamId, const QByteArray& tenantId) const = 0;

	/**
		Get parent team ID (for nested teams).
		\return Parent team ID, or empty if the team has no parent.
	*/
	virtual QByteArray GetParentTeam(const QByteArray& teamId) const = 0;

	/**
		Get child team IDs (for nested teams).
	*/
	virtual QByteArrayList GetChildTeams(const QByteArray& teamId) const = 0;
};


} // namespace imtauth
