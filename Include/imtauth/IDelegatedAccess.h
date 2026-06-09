// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QByteArray>
#include <QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	How a user gains access to a particular tenant.
	\ingroup Permission
*/
enum TenantAccessType
{
	TAT_NONE = 0,		///< No access to the tenant.
	TAT_MEMBERSHIP,		///< Direct membership (the user belongs to the tenant).
	TAT_DELEGATED		///< Delegated access via an effective CrossOrgGrant.
};


/**
	Describes the resolved access a user has to a single tenant, combining
	direct membership and delegated cross-org grants.

	For TAT_DELEGATED entries, \c delegatedRoleIds holds the union of roles
	delegated through effective grants and defines the scope of the delegated
	session. For TAT_MEMBERSHIP and TAT_NONE, \c delegatedRoleIds is empty.
*/
struct TenantAccessInfo
{
	QByteArray tenantId;
	TenantAccessType accessType = TAT_NONE;
	QByteArrayList delegatedRoleIds;

	bool operator==(const TenantAccessInfo& other) const
	{
		return tenantId == other.tenantId
			&& accessType == other.accessType
			&& delegatedRoleIds == other.delegatedRoleIds;
	}

	bool operator!=(const TenantAccessInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<TenantAccessInfo> TenantAccessList;


/**
	Resolves the full delegated-access picture by combining direct tenant
	memberships with cross-org grants.

	This is the single entry point used by organization navigation
	(switcher), delegated context switching, and permission checking, so that
	those layers do not have to combine ITenantMembershipManager and
	ICrossOrgGrant themselves.

	\ingroup Permission
*/
class IDelegatedAccess: virtual public istd::IPolymorphic
{
public:
	/**
		Get every tenant a user can currently operate in from within
		\c currentTenantId: the user's own memberships plus any tenants
		reachable through an effective cross-org grant to \c currentTenantId.

		Membership entries take precedence over delegated entries for the same
		tenant, so each tenant appears at most once. Delegated entries carry
		the granted role scope in \c delegatedRoleIds.
	*/
	virtual TenantAccessList GetAccessibleTenants(
		const QByteArray& userId,
		const QByteArray& currentTenantId) const = 0;

	/**
		Resolve how the user would access \c targetTenantId while operating in
		\c currentTenantId. Returns a TAT_MEMBERSHIP entry when the user is a
		member of the target, a TAT_DELEGATED entry (with granted roles) when an
		effective grant from the target to the current tenant exists, or a
		TAT_NONE entry otherwise.
	*/
	virtual TenantAccessInfo ResolveTenantAccess(
		const QByteArray& userId,
		const QByteArray& currentTenantId,
		const QByteArray& targetTenantId) const = 0;

	/**
		Check whether the user may perform an action requiring \c requiredRoleId
		against \c targetTenantId while operating in \c currentTenantId.

		Membership access defers to the user's personal rights in the target
		tenant (returns true). Delegated access is allowed only when the
		required role is within the grant scope; an empty role is always allowed
		for delegated access. No access returns false.
	*/
	virtual bool IsAccessAllowed(
		const QByteArray& userId,
		const QByteArray& currentTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& requiredRoleId) const = 0;
};


} // namespace imtauth
