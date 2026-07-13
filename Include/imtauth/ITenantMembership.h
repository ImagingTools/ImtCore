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
		Get additional organization-specific permissions granted directly to this membership (member within the org).
		These control the capabilities (replacing the old EnvironmentRole concept).
		Typically populated from the OrganizationsPermissionsProvider (e.g. EditOrganizationMember, ViewOrganizationRoles, etc.).
	*/
	virtual QByteArrayList GetOrganizationPermissions() const = 0;

	/**
		Set additional organization-specific permissions for this membership.
	*/
	virtual void SetOrganizationPermissions(const QByteArrayList& permissions) = 0;

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
