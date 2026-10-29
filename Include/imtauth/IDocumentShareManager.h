// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/IDocumentShareInfo.h>


namespace imtauth
{


typedef QList<IDocumentShareInfoSharedPtr> DocumentShares;


/**
	Interface for managing document shares between tenants.
	A document can be shared with a tenant only once; sharing it again updates the existing share.
	\ingroup Tenant
*/
class IDocumentShareManager: virtual public istd::IChangeable
{
public:
	/**
		Share a document with the target tenant of \a shareInfo, or update the existing share.
		The owner and the target tenant must be different and related by an active tenant relationship.
		The share ID, the status, the target groups and the tenant names of \a shareInfo are set by the manager.
		\return Share ID, empty if the share was refused.
	*/
	virtual QByteArray ShareDocument(const IDocumentShareInfo& shareInfo, QString* errorMessagePtr = nullptr) = 0;

	/**
		Remove shares. The documents stay untouched.
	*/
	virtual bool RemoveShares(const QByteArrayList& shareIds) = 0;

	/**
		Set the status of shares (accept or decline them on the target side).
	*/
	virtual bool SetShareStatus(const QByteArrayList& shareIds, IDocumentShareInfo::ShareStatus status) = 0;

	/**
		Restrict a share to members of the given target tenant groups. An empty list removes the restriction.
	*/
	virtual bool SetShareTargetGroups(const QByteArray& shareId, const QByteArrayList& groupIds) = 0;

	/**
		Get a share by its ID. Returns an invalid pointer if the share does not exist.
	*/
	virtual IDocumentShareInfoUniquePtr GetShare(const QByteArray& shareId) const = 0;

	/**
		Get all shares of a document.
	*/
	virtual DocumentShares GetDocumentShares(const QByteArray& entityId) const = 0;

	/**
		Get all shares received by a tenant.
	*/
	virtual DocumentShares GetIncomingShares(const QByteArray& targetTenantId) const = 0;

	/**
		Get all shares given by a tenant.
	*/
	virtual DocumentShares GetOutgoingShares(const QByteArray& ownerTenantId) const = 0;

	/**
		Organizations whose shares are effective for the tenant: the ones it has a relationship with that is valid now.
		A share of a relationship outside its validity period grants nothing but stays.
		Reading the relationships anew removes the shares of the ended ones, as RemoveUnrelatedShares() does.
	*/
	virtual QByteArrayList GetShareOwnerTenantIds(const QByteArray& targetTenantId) const = 0;

	/**
		Remove the given and received shares of a tenant with the organizations it has no active relationship with any more,
		or that are left from an earlier period of relationships.
		Nothing is removed when the relationships of the tenant cannot be read.
		The relationships read are reused for a while, a repeated call within that time removes nothing.
		\return The number of the removed shares.
	*/
	virtual int RemoveUnrelatedShares(const QByteArray& tenantId) = 0;

	/**
		Number that changes whenever a share is stored or removed.
		Results computed from the shares, e.g. cached document lists, are outdated once it changed.
	*/
	virtual int GetSharesRevision() const = 0;
};


} // namespace imtauth


