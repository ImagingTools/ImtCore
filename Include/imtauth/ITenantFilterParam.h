// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QByteArrayList>

// ImtCore includes
#include <imtauth/IDocumentShareInfo.h>


namespace imtauth
{


/**
	Tenant filter mode: Include returns items belonging to the tenant,
	Exclude returns items NOT belonging to the tenant.
*/
enum TenantFilterMode
{
	TFM_INCLUDE = 0,
	TFM_EXCLUDE = 1
};


/**
	Which documents the filter selects with respect to document sharing.
	SS_OWN: documents of the tenant (default).
	SS_ALL: documents of the tenant and documents shared with it.
	SS_INCOMING: only documents shared with the tenant.
	SS_OUTGOING: only documents of the tenant shared with other tenants.
	SS_ACCESSIBLE: every document the tenant may access: its own, the ones of no tenant and the ones shared with it.
*/
enum SharingScope
{
	SS_OWN = 0,
	SS_ALL,
	SS_INCOMING,
	SS_OUTGOING,
	SS_ACCESSIBLE
};


class ITenantFilterParam: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	virtual QByteArray GetOwnerId() const = 0;
	virtual void SetOwnerId(const QByteArray& ownerId) = 0;

	virtual TenantFilterMode GetFilterMode() const = 0;
	virtual void SetFilterMode(TenantFilterMode mode) = 0;

	virtual SharingScope GetSharingScope() const = 0;
	virtual void SetSharingScope(SharingScope scope) = 0;

	/**
		Lowest access level a share must grant to select a document shared with the tenant.
	*/
	virtual IDocumentShareInfo::AccessLevel GetMinimumAccessLevel() const = 0;
	virtual void SetMinimumAccessLevel(IDocumentShareInfo::AccessLevel accessLevel) = 0;

	/**
		Groups of the requesting user. A share restricted to target groups selects the document
		only for members of these groups, and only while the restriction is enabled.
	*/
	virtual QByteArrayList GetUserGroupIds() const = 0;
	virtual bool IsGroupRestrictionEnabled() const = 0;
	virtual void SetUserGroupIds(const QByteArrayList& groupIds, bool restrictionEnabled) = 0;

	/**
		Organizations whose shares are effective for the tenant, i.e. the ones it currently has an active relationship with.
		While the restriction is enabled a document shared by any other organization is not selected.
	*/
	virtual QByteArrayList GetShareOwnerTenantIds() const = 0;
	virtual bool IsShareOwnerRestrictionEnabled() const = 0;
	virtual void SetShareOwnerTenantIds(const QByteArrayList& tenantIds, bool restrictionEnabled) = 0;

	/**
		Revision of the document shares (IDocumentShareManager::GetSharesRevision) the filter was made for.
		It is part of the filter, so a result cached for older shares is not reused.
	*/
	virtual int GetSharesRevision() const = 0;
	virtual void SetSharesRevision(int revision) = 0;
};


} // namespace imtauth
