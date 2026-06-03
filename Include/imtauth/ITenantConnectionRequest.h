// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QByteArray>
#include <QString>
#include <QList>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


/**
	Lifecycle status of a tenant connection request.
*/
enum TenantConnectionStatus
{
	TCS_PENDING = 0,
	TCS_ACCEPTED,
	TCS_REJECTED,
	TCS_EXPIRED,
	TCS_REVOKED
};


/**
	Structure describing a tenant connection request.

	Connection requests implement the discovery/bootstrap workflow that lets two
	otherwise isolated tenants establish their first contact. A request can be
	addressed directly to another tenant (via \a targetIdentifier, e.g. an
	e-mail or slug) or distributed out-of-band as a one-time \a connectCode.

	When the request is accepted an asymmetric relationship is established
	between the source and the accepting tenant using \a proposedSourceRole and
	\a proposedTargetRole.
*/
struct TenantConnectionRequestInfo
{
	QByteArray requestId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QString targetIdentifier;
	QString connectCode;
	ITenantInfo::TenantRelationshipRole proposedSourceRole = ITenantInfo::Partner;
	ITenantInfo::TenantRelationshipRole proposedTargetRole = ITenantInfo::Partner;
	QString message;
	TenantConnectionStatus status = TCS_PENDING;
	QString createdAt;
	QString expiresAt;
	QString respondedAt;

	bool operator==(const TenantConnectionRequestInfo& other) const
	{
		return requestId == other.requestId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& targetIdentifier == other.targetIdentifier
			&& connectCode == other.connectCode
			&& proposedSourceRole == other.proposedSourceRole
			&& proposedTargetRole == other.proposedTargetRole
			&& message == other.message
			&& status == other.status
			&& createdAt == other.createdAt
			&& expiresAt == other.expiresAt
			&& respondedAt == other.respondedAt;
	}

	bool operator!=(const TenantConnectionRequestInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<TenantConnectionRequestInfo> TenantConnectionRequests;


/**
	Interface for managing tenant connection requests (discovery workflow).

	Provides the mechanism for tenants to find each other and establish a first
	relationship through explicit requests or one-time connect codes. Accepting a
	request creates an asymmetric tenant relationship between the two parties.

	\ingroup Tenant
*/
class ITenantConnectionRequest: virtual public istd::IChangeable
{
public:
	/**
		Create a connection request addressed to another tenant.
		\param sourceTenantId Tenant initiating the request.
		\param targetIdentifier External identifier of the target tenant (e-mail/slug).
		\param proposedSourceRole Role of the source tenant in the resulting relationship.
		\param proposedTargetRole Role of the target tenant in the resulting relationship.
		\param message Optional human-readable message.
		\param expiresAt Optional expiry timestamp (empty for no expiry).
		\return Request ID if successful, empty if failed.
	*/
	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& targetIdentifier,
				ITenantInfo::TenantRelationshipRole proposedSourceRole,
				ITenantInfo::TenantRelationshipRole proposedTargetRole,
				const QString& message = QString(),
				const QString& expiresAt = QString()) = 0;

	/**
		Create a one-time connect code that any tenant can redeem to connect.
		\param sourceTenantId Tenant generating the code.
		\param proposedSourceRole Role of the source tenant in the resulting relationship.
		\param proposedTargetRole Role of the redeeming tenant in the resulting relationship.
		\param message Optional human-readable message.
		\param expiresAt Optional expiry timestamp (empty for no expiry).
		\param generatedCode Output parameter receiving the generated connect code.
		\return Request ID if successful, empty if failed.
	*/
	virtual QByteArray CreateConnectCode(
				const QByteArray& sourceTenantId,
				ITenantInfo::TenantRelationshipRole proposedSourceRole,
				ITenantInfo::TenantRelationshipRole proposedTargetRole,
				const QString& message,
				const QString& expiresAt,
				QString& generatedCode) = 0;

	/**
		Accept a pending connection request by its ID.
		Establishes the asymmetric relationship between the source and the
		accepting tenant.
		\param requestId The request to accept.
		\param acceptingTenantId The tenant accepting the request.
		\return true if accepted successfully.
	*/
	virtual bool AcceptConnectionRequest(const QByteArray& requestId, const QByteArray& acceptingTenantId) = 0;

	/**
		Accept a pending connection request by its connect code.
		\param connectCode The connect code to redeem.
		\param acceptingTenantId The tenant redeeming the code.
		\return The accepted request ID, or empty on failure.
	*/
	virtual QByteArray AcceptConnectCode(const QString& connectCode, const QByteArray& acceptingTenantId) = 0;

	/**
		Reject a pending connection request.
		\return true if rejected successfully.
	*/
	virtual bool RejectConnectionRequest(const QByteArray& requestId) = 0;

	/**
		Revoke a connection request issued by the source tenant.
		\return true if revoked successfully.
	*/
	virtual bool RevokeConnectionRequest(const QByteArray& requestId) = 0;

	/**
		Delete (physically remove) a connect code request by its ID.
		Typically used by the originating tenant to revoke and remove the code
		from history.
		\return true if the request was removed.
	*/
	virtual bool DeleteConnectCode(const QByteArray& requestId) = 0;

	/**
		Purge expired connect-code requests for the specified source tenant.
		\return Number of removed expired requests.
	*/
	virtual int PurgeExpiredConnectCodes(const QByteArray& sourceTenantId) = 0;

	/**
		Get a specific connection request by its ID.
	*/
	virtual TenantConnectionRequestInfo GetConnectionRequest(const QByteArray& requestId) const = 0;

	/**
		Get all connection requests issued by the specified source tenant.
	*/
	virtual TenantConnectionRequests GetOutgoingRequests(const QByteArray& sourceTenantId) const = 0;

	/**
		Get all connection requests addressed to the specified target tenant
		(either resolved by ID or by its external identifier).
	*/
	virtual TenantConnectionRequests GetIncomingRequests(
				const QByteArray& targetTenantId,
				const QString& targetIdentifier = QString()) const = 0;

	/**
		Get a pending connection request by its connect code.
	*/
	virtual TenantConnectionRequestInfo GetRequestByCode(const QString& connectCode) const = 0;
};


} // namespace imtauth
