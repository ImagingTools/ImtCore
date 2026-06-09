// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QByteArray>
#include <QString>
#include <QList>

// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


/**
	Structure describing a cross-organization grant.
	Represents a specific permission delegation from one tenant to another,
	scoped by assigned roles with optional expiry.
*/
struct CrossOrgGrantInfo
{
	QByteArray grantId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QString name;
	QByteArrayList roleIds;
	QString description;
	QString createdAt;
	QString expiresAt;
	bool isActive;

	bool operator==(const CrossOrgGrantInfo& other) const
	{
		return grantId == other.grantId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& name == other.name
			&& roleIds == other.roleIds
			&& description == other.description
			&& createdAt == other.createdAt
			&& expiresAt == other.expiresAt
			&& isActive == other.isActive;
	}

	bool operator!=(const CrossOrgGrantInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<CrossOrgGrantInfo> CrossOrgGrants;


/**
	Interface for managing cross-organization grants.
	Provides the mechanism to delegate specific permissions from one tenant
	to another, supporting B2B collaboration scenarios.

	Grants delegate selected roles from the source tenant to the target
	tenant. The parent/system tenant does NOT have implicit access to child
	tenant data — access must be explicitly granted.

	\ingroup Permission
*/
class ICrossOrgGrant: virtual public istd::IChangeable
{
public:
	/**
		Create a new cross-org grant.
		\param sourceTenantId Tenant granting access.
		\param targetTenantId Tenant receiving access.
		\param roleIds List of role IDs being delegated.
		\param description Human-readable description of the grant.
		\param expiresAt Optional expiry timestamp (empty for no expiry).
		\return Grant ID if successful, empty if failed.
	*/
	virtual QByteArray CreateGrant(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArrayList& roleIds,
		const QString& description = QString(),
		const QString& expiresAt = QString()) = 0;

	/**
		Remove a cross-org grant by its ID.
		\return true if removed successfully.
	*/
	virtual bool RevokeGrant(const QByteArray& grantId) = 0;

	/**
		Permanently remove cross-org grants by their IDs.
		\return true if all requested grants were removed successfully.
	*/
	virtual bool RemoveGrants(const QByteArrayList& grantIds) = 0;

	/**
		Get a specific grant by its ID.
	*/
	virtual CrossOrgGrantInfo GetGrant(const QByteArray& grantId) const = 0;

	/**
		Get all grants where the specified tenant is the source (granting access).
	*/
	virtual CrossOrgGrants GetGrantsBySourceTenant(const QByteArray& sourceTenantId) const = 0;

	/**
		Get all grants where the specified tenant is the target (receiving access).
	*/
	virtual CrossOrgGrants GetGrantsByTargetTenant(const QByteArray& targetTenantId) const = 0;

	/**
		Check if a target tenant has at least one of the specified roles
		granted by the source tenant.
	*/
	virtual bool HasAccess(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& roleId) const = 0;

	/**
		Get the distinct list of source tenants that the specified target
		tenant can reach through at least one currently effective grant
		(active and not expired).

		These are the organizations a user of the target tenant should see
		in the organization switcher in addition to its own memberships.
		The returned tenants are accessible via delegated access only.
	*/
	virtual QByteArrayList GetDelegatedSourceTenants(const QByteArray& targetTenantId) const = 0;

	/**
		Get the union of all role IDs delegated from the source tenant to the
		target tenant through currently effective grants (active and not
		expired). Duplicate roles across multiple grants are returned once.

		These are the roles that are active when the target tenant operates
		inside the source tenant via delegated access, and define the scope of
		what such a delegated session is allowed to do.
	*/
	virtual QByteArrayList GetGrantedRoles(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId) const = 0;

	/**
		Check whether the target tenant currently has delegated access to the
		source tenant, i.e. at least one effective grant exists between them.

		Used to distinguish delegated organizations from regular membership
		organizations in the UI and to validate a delegated context switch.
	*/
	virtual bool IsDelegatedAccess(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId) const = 0;
};


} // namespace imtauth
