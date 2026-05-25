// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QList>
#include <QString>


namespace imtauth
{


/**
	Environment roles for tenant members.
	Determines the level of access a user has within a tenant.
	Priority order: Creator > Owner > Admin > Member.
*/
enum TenantEnvironmentRole
{
	TER_MEMBER = 0,		///< Read-only access, can only leave the organization
	TER_ADMIN,			///< Can manage members (invite/remove) and change roles
	TER_OWNER,			///< Superuser within the tenant (all permissions except changing Creator)
	TER_CREATOR			///< Immutable creator — full access including Permissions management
};


/**
	Convert a TenantEnvironmentRole enum value to its string representation.
*/
inline QString TenantEnvironmentRoleToString(TenantEnvironmentRole role)
{
	switch (role){
		case TER_CREATOR:	return QStringLiteral("Creator");
		case TER_OWNER:		return QStringLiteral("Owner");
		case TER_ADMIN:		return QStringLiteral("Admin");
		case TER_MEMBER:
		default:			return QStringLiteral("Member");
	}
}


/**
	Convert a string to a TenantEnvironmentRole enum value.
	Unrecognized strings default to TER_MEMBER.
*/
inline TenantEnvironmentRole TenantEnvironmentRoleFromString(const QString& roleStr)
{
	if (roleStr == QLatin1String("Creator"))	return TER_CREATOR;
	if (roleStr == QLatin1String("Owner"))		return TER_OWNER;
	if (roleStr == QLatin1String("Admin"))		return TER_ADMIN;
	return TER_MEMBER;
}


/**
	Interface describing a membership — the relationship between a User and a Tenant.
	This is the missing entity that explicitly models "User X belongs to Tenant Y with Role Z".

	Analogous to GitHub's Organization Membership.
	\ingroup TenantMembership
*/
class ITenantMembership: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_MEMBERSHIP_ID = idoc::IDocumentMetaInfo::MIT_USER + 300,
		MIT_MEMBERSHIP_USER_ID,
		MIT_MEMBERSHIP_TENANT_ID,
		MIT_MEMBERSHIP_ROLE,
		MIT_MEMBERSHIP_JOINED_AT
	};

	/**
		Get the unique membership ID.
	*/
	virtual QByteArray GetMembershipId() const = 0;

	/**
		Set the unique membership ID.
	*/
	virtual void SetMembershipId(const QByteArray& membershipId) = 0;

	/**
		Get the user ID associated with this membership.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set the user ID.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get the tenant (organization) ID.
	*/
	virtual QByteArray GetTenantId() const = 0;

	/**
		Set the tenant (organization) ID.
	*/
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	/**
		Get the environment role of the user within the tenant.
	*/
	virtual TenantEnvironmentRole GetEnvironmentRole() const = 0;

	/**
		Set the environment role of the user within the tenant.
	*/
	virtual void SetEnvironmentRole(TenantEnvironmentRole role) = 0;

	/**
		Get the role as a string ID (for backward compatibility with DB/SDL).
		Returns the string representation of the environment role.
	*/
	virtual QByteArray GetRoleId() const = 0;

	/**
		Set the role from a string ID (for backward compatibility with DB/SDL).
		Parses the string and sets the environment role accordingly.
	*/
	virtual void SetRoleId(const QByteArray& roleId) = 0;

	/**
		Get the date when the user joined the tenant.
	*/
	virtual QString GetJoinedAt() const = 0;

	/**
		Set the date when the user joined the tenant.
	*/
	virtual void SetJoinedAt(const QString& joinedAt) = 0;

	/**
		Check if the membership is active.
	*/
	virtual bool IsActive() const = 0;

	/**
		Set active state of the membership.
	*/
	virtual void SetActive(bool isActive) = 0;

	/**
		Get the last update timestamp.
	*/
	virtual QString GetUpdatedAt() const = 0;

	/**
		Set the last update timestamp.
	*/
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantMembership> ITenantMembershipUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantMembership> ITenantMembershipSharedPtr;


} // namespace imtauth
