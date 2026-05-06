// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QList>


namespace imtauth
{


/**
	Defines the role of a user within a tenant (organization).
	Analogous to GitHub's organization membership roles.
*/
enum TenantMemberRole
{
	/**
		Full control over the tenant: settings, billing, members, teams.
		Analogous to GitHub Organization Owner.
	*/
	TMR_Owner = 0,

	/**
		Can manage members, teams, and most settings but cannot delete the tenant.
		Analogous to GitHub admin permissions at org level.
	*/
	TMR_Admin,

	/**
		Standard member with access determined by team assignments.
		Analogous to GitHub Organization Member.
	*/
	TMR_Member,

	/**
		Limited access, read-only by default.
		Analogous to GitHub outside collaborator with minimal permissions.
	*/
	TMR_Guest
};


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
		Get the role of the user within the tenant.
	*/
	virtual TenantMemberRole GetRole() const = 0;

	/**
		Set the role of the user within the tenant.
	*/
	virtual void SetRole(TenantMemberRole role) = 0;

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
};


typedef istd::TUniqueInterfacePtr<ITenantMembership> ITenantMembershipUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantMembership> ITenantMembershipSharedPtr;


} // namespace imtauth
