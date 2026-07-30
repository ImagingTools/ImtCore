// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QByteArray>
#include <QString>
#include <QList>


namespace imtauth
{


/**
	Access mode granted to the target tenant of a document share.

	\a DSAM_READ_ONLY allows the target tenant to read the shared document only,
	\a DSAM_FULL_ACCESS additionally allows it to modify the document.
*/
enum DocumentShareAccessMode
{
	DSAM_READ_ONLY = 0,
	DSAM_FULL_ACCESS
};


/**
	Lifecycle status of a document share.
*/
enum DocumentShareStatus
{
	DSS_ACTIVE = 0,
	DSS_REVOKED,
	DSS_EXPIRED
};


/**
	Structure describing a single document share.

	A document share is an explicit, revocable grant issued by the owning
	(source) tenant to another (target) tenant for exactly one document. It is
	an independent entity: it neither extends nor depends on the cross-tenant
	messaging pipeline. The share carries the access mode (read-only or full
	access) which the consuming side has to enforce whenever the shared document
	is opened.
*/
struct DocumentShareInfo
{
	QByteArray shareId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray documentId;
	QString documentType;
	QString documentName;
	DocumentShareAccessMode accessMode = DSAM_READ_ONLY;
	DocumentShareStatus status = DSS_ACTIVE;
	QByteArray sharedByUserId;
	QString createdAt;
	QString updatedAt;
	QString expiresAt;
	QString revokedAt;

	bool operator==(const DocumentShareInfo& other) const
	{
		return shareId == other.shareId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& relationshipId == other.relationshipId
			&& documentId == other.documentId
			&& documentType == other.documentType
			&& documentName == other.documentName
			&& accessMode == other.accessMode
			&& status == other.status
			&& sharedByUserId == other.sharedByUserId
			&& createdAt == other.createdAt
			&& updatedAt == other.updatedAt
			&& expiresAt == other.expiresAt
			&& revokedAt == other.revokedAt;
	}

	bool operator!=(const DocumentShareInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<DocumentShareInfo> DocumentShares;


/**
	Interface of the document share manager.

	The manager owns the document share entities: it creates them, keeps them in
	a collection, updates the granted access mode, revokes them and answers
	access questions for the consuming side.

	\ingroup Tenant
*/
class IDocumentShareManager: virtual public istd::IChangeable
{
public:
	/**
		Create a new document share.

		The manager validates the mandatory fields of \a shareInfo (source
		tenant, target tenant and document), assigns a new share ID, sets the
		status to DSS_ACTIVE and stores the share.

		\param shareInfo Description of the share to create. The \a shareId,
			\a status, \a createdAt, \a updatedAt and \a revokedAt members are
			assigned by the manager and can be left empty.
		\return Share ID if successful, empty if validation or storage failed.
	*/
	virtual QByteArray CreateShare(const DocumentShareInfo& shareInfo) = 0;

	/**
		Get a specific document share by its ID.
	*/
	virtual DocumentShareInfo GetShare(const QByteArray& shareId) const = 0;

	/**
		Get all shares issued to the specified target tenant.
	*/
	virtual DocumentShares GetIncomingShares(const QByteArray& targetTenantId) const = 0;

	/**
		Get all shares issued by the specified source tenant.
	*/
	virtual DocumentShares GetOutgoingShares(const QByteArray& sourceTenantId) const = 0;

	/**
		Get all shares issued for the specified document.
	*/
	virtual DocumentShares GetSharesOfDocument(const QByteArray& documentId) const = 0;

	/**
		Change the access mode granted by an existing share.
		\return true if updated successfully.
	*/
	virtual bool SetAccessMode(const QByteArray& shareId, DocumentShareAccessMode accessMode) = 0;

	/**
		Revoke an existing share. A revoked share no longer grants any access.
		\return true if updated successfully.
	*/
	virtual bool RevokeShare(const QByteArray& shareId) = 0;

	/**
		Resolve the access a tenant currently has on a document.

		Only active, non-expired shares are taken into account. If several
		shares exist for the same tenant and document, the most permissive
		access mode is returned.

		\param tenantId Tenant asking for access.
		\param documentId Document to be accessed.
		\param accessModePtr Optional output of the granted access mode. It is
			only written when access is granted.
		\return true if the tenant currently has access to the document.
	*/
	virtual bool GetDocumentAccess(
				const QByteArray& tenantId,
				const QByteArray& documentId,
				DocumentShareAccessMode* accessModePtr = nullptr) const = 0;
};


} // namespace imtauth
