// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QByteArray>
#include <QByteArrayList>
#include <QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Describes a single organization accessible to a user through delegated
	access (CrossOrgGrant), as opposed to direct membership.
*/
struct DelegatedTenantInfo
{
	QByteArray tenantId;
	QByteArray grantId;
	QByteArrayList roleIds;
	bool isDelegated;

	bool operator==(const DelegatedTenantInfo& other) const
	{
		return tenantId == other.tenantId
			&& grantId == other.grantId
			&& roleIds == other.roleIds
			&& isDelegated == other.isDelegated;
	}

	bool operator!=(const DelegatedTenantInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<DelegatedTenantInfo> DelegatedTenantList;


/**
	Interface for resolving delegated (cross-org) access.

	Composes ITenantMembershipManager and ICrossOrgGrant to provide:
	- Unified list of accessible organizations (membership + grants)
	- Permission checking that accounts for cross-org grants
	- Context resolution for tenant switching with delegated roles

	\ingroup Permission
*/
class IDelegatedAccess: virtual public istd::IPolymorphic
{
public:
	/**
		Get all tenant IDs accessible to a user, combining direct memberships
		and tenants accessible through active cross-org grants.
		\param userId The user requesting access.
		\param homeTenantId The user's home tenant (used to look up grants targeting this tenant).
		\return List of accessible tenants with delegation metadata.
	*/
	virtual DelegatedTenantList GetAccessibleTenants(
		const QByteArray& userId,
		const QByteArray& homeTenantId) const = 0;

	/**
		Check if a user has the specified role in a tenant, considering both
		direct membership roles and roles delegated through cross-org grants.
		\param userId The user to check.
		\param homeTenantId The user's home tenant.
		\param targetTenantId The tenant to check access for.
		\param roleId The role to verify. Empty roleId means "any access".
		\return true if the user has the role via membership or delegation.
	*/
	virtual bool HasDelegatedRole(
		const QByteArray& userId,
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& roleId) const = 0;

	/**
		Get the effective delegated roles for a user in a specific tenant.
		Returns only roles obtained through cross-org grants (not membership roles).
		\param homeTenantId The user's home tenant.
		\param targetTenantId The tenant the user is accessing.
		\return List of role IDs delegated through active grants.
	*/
	virtual QByteArrayList GetDelegatedRoles(
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId) const = 0;

	/**
		Check if access to a target tenant is obtained via delegation (cross-org grant)
		rather than direct membership.
		\param userId The user to check.
		\param homeTenantId The user's home tenant.
		\param targetTenantId The tenant to check.
		\return true if access is through delegation only (not direct membership).
	*/
	virtual bool IsDelegatedAccess(
		const QByteArray& userId,
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId) const = 0;

	/**
		Get all roles delegated to a user through active cross-org grants.
		The user's home tenants are resolved from the direct memberships and
		the roles of all effective grants targeting these tenants are aggregated.
		\param userId The user to resolve the delegated roles for.
		\return Deduplicated list of role IDs delegated through active grants.
	*/
	virtual QByteArrayList GetDelegatedUserRoles(const QByteArray& userId) const = 0;
};


} // namespace imtauth
