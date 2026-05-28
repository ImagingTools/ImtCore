// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QByteArray>
#include <QString>
#include <QList>


namespace imtauth
{


/**
	Access level for cross-tenant grants.
	Defines what level of access a grant provides between tenants.
*/
enum CrossOrgAccessLevel
{
	COAL_NONE = 0,
	COAL_READ,
	COAL_WRITE,
	COAL_ADMIN
};


/**
	Structure describing a cross-organization grant.
	Represents a specific permission delegation from one tenant to another,
	optionally scoped to a team/group and a specific resource scope.
*/
struct CrossOrgGrantInfo
{
	QByteArray grantId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray targetTeamId;
	CrossOrgAccessLevel accessLevel;
	QString resourceScope;
	QString description;
	QString createdAt;
	QString expiresAt;
	bool isActive;

	bool operator==(const CrossOrgGrantInfo& other) const
	{
		return grantId == other.grantId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& relationshipId == other.relationshipId
			&& targetTeamId == other.targetTeamId
			&& accessLevel == other.accessLevel
			&& resourceScope == other.resourceScope
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

	Grants are always tied to an existing tenant relationship and require
	explicit delegation from the source tenant. The parent/system tenant
	does NOT have implicit access to child tenant data — access must be
	explicitly granted.

	\ingroup Permission
*/
class ICrossOrgGrant: virtual public istd::IPolymorphic
{
public:
	/**
		Create a new cross-org grant.
		\param sourceTenantId Tenant granting access.
		\param targetTenantId Tenant receiving access.
		\param relationshipId The relationship that this grant is associated with.
		\param accessLevel The level of access being granted.
		\param resourceScope Optional scope limiting the grant to specific resources.
		\param targetTeamId Optional team in the target tenant that receives the grant.
		\param description Human-readable description of the grant.
		\param expiresAt Optional expiry timestamp (empty for no expiry).
		\return Grant ID if successful, empty if failed.
	*/
	virtual QByteArray CreateGrant(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& relationshipId,
		CrossOrgAccessLevel accessLevel,
		const QString& resourceScope = QString(),
		const QByteArray& targetTeamId = QByteArray(),
		const QString& description = QString(),
		const QString& expiresAt = QString()) = 0;

	/**
		Remove a cross-org grant by its ID.
		\return true if removed successfully.
	*/
	virtual bool RevokeGrant(const QByteArray& grantId) = 0;

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
		Get all grants associated with a specific relationship.
	*/
	virtual CrossOrgGrants GetGrantsByRelationship(const QByteArray& relationshipId) const = 0;

	/**
		Check if a target tenant has at least the required access level
		for a given resource scope from the source tenant.
	*/
	virtual bool HasAccess(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QString& resourceScope,
		CrossOrgAccessLevel requiredLevel) const = 0;
};


} // namespace imtauth
