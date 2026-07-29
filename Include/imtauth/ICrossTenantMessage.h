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
	Type of a cross-tenant message.
	Identifies the domain semantics of a message exchanged through the broker.
	\a CTMT_CUSTOM is used for application-specific message types; in that case
	the concrete type name is carried in CrossTenantMessageInfo::customType.
*/
enum CrossTenantMessageType
{
	CTMT_CUSTOM = 0,
	CTMT_ORDER_REQUEST,
	CTMT_ORDER_CONFIRMATION,
	CTMT_ORDER_REJECTION,
	CTMT_ORDER_STATUS_UPDATE,
	CTMT_ORDER_CANCELLATION,
	CTMT_DOCUMENT_SHARE
};


/**
	Access mode for document sharing.
	Determines whether the recipient has read-only or full (edit) access
	to the shared document.
*/
enum DocumentShareAccessMode
{
	DSAM_READ_ONLY = 0,
	DSAM_FULL_ACCESS
};


/**
	Lifecycle status of a cross-tenant message.
*/
enum CrossTenantMessageStatus
{
	CTMS_CREATED = 0,
	CTMS_VALIDATED,
	CTMS_DELIVERED,
	CTMS_ACKNOWLEDGED,
	CTMS_PROCESSED,
	CTMS_FAILED,
	CTMS_EXPIRED
};


/**
	Structure describing a single cross-tenant message.

	A cross-tenant message is the unit of exchange of the inter-tenant messaging
	pipeline. It never grants direct access to another tenant's data: the source
	tenant produces a message from its own object, the broker validates the
	tenant relationship and applies a projection to the payload, and a domain
	handler in the target tenant materializes a brand-new local object from the
	projected payload. Both tenants only ever work with their own local objects,
	linked through \a sourceObjectId and \a targetObjectId.
*/
struct CrossTenantMessageInfo
{
	QByteArray messageId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray contractId;
	QByteArray sourceObjectId;
	QByteArray targetObjectId;
	CrossTenantMessageType messageType = CTMT_CUSTOM;
	QString customType;
	QByteArray payload;
	CrossTenantMessageStatus status = CTMS_CREATED;
	QString errorMessage;
	DocumentShareAccessMode accessMode = DSAM_READ_ONLY;
	QString createdAt;
	QString updatedAt;
	QString expiresAt;

	bool operator==(const CrossTenantMessageInfo& other) const
	{
		return messageId == other.messageId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& relationshipId == other.relationshipId
			&& contractId == other.contractId
			&& sourceObjectId == other.sourceObjectId
			&& targetObjectId == other.targetObjectId
			&& messageType == other.messageType
			&& customType == other.customType
			&& payload == other.payload
			&& status == other.status
			&& errorMessage == other.errorMessage
			&& accessMode == other.accessMode
			&& createdAt == other.createdAt
			&& updatedAt == other.updatedAt
			&& expiresAt == other.expiresAt;
	}

	bool operator!=(const CrossTenantMessageInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<CrossTenantMessageInfo> CrossTenantMessages;


/**
	Interface of the cross-tenant message broker.

	The broker is the central, domain-agnostic component of the messaging core
	(RFC phase 2). It accepts cross-tenant requests, validates them against the
	tenant relationship, applies the configured projection to the payload, stores
	the message and dispatches it to the registered domain handlers. It does not
	understand the business semantics of any concrete message; the meaning is
	provided by pluggable ICrossTenantMessageHandler implementations.

	\ingroup Tenant
*/
class ICrossTenantMessage: virtual public istd::IChangeable
{
public:
	/**
		Send a new cross-tenant message.

		The broker validates that an active relationship identified by
		\a relationshipId exists between \a sourceTenantId and \a targetTenantId,
		applies the projection to \a payload and stores the resulting message. If
		domain handlers are registered they are dispatched synchronously and the
		message status is advanced accordingly.

		\param sourceTenantId Tenant producing the message.
		\param targetTenantId Tenant the message is addressed to.
		\param relationshipId Relationship authorizing the exchange.
		\param messageType Domain type of the message.
		\param payload Source payload (e.g. a serialized domain object). The broker
			stores the projected payload, not the raw one.
		\param sourceObjectId Optional ID of the source object the message was
			produced from.
		\param customType Concrete type name when \a messageType is CTMT_CUSTOM.
		\param expiresAt Optional expiry timestamp (empty for no expiry).
		\param contractId Optional contract that authorizes this exchange (empty for ad-hoc messages).
		\param accessMode Access mode for document sharing (read-only or full access).
		\return Message ID if successful, empty if validation or storage failed.
	*/
	virtual QByteArray SendMessage(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& relationshipId,
				CrossTenantMessageType messageType,
				const QByteArray& payload,
				const QByteArray& sourceObjectId = QByteArray(),
				const QString& customType = QString(),
				const QString& expiresAt = QString(),
				const QByteArray& contractId = QByteArray(),
				DocumentShareAccessMode accessMode = DSAM_READ_ONLY) = 0;

	/**
		Get a specific message by its ID.
	*/
	virtual CrossTenantMessageInfo GetMessage(const QByteArray& messageId) const = 0;

	/**
		Get all messages addressed to the specified target tenant (inbox).
	*/
	virtual CrossTenantMessages GetIncomingMessages(const QByteArray& targetTenantId) const = 0;

	/**
		Get all messages produced by the specified source tenant (outbox).
	*/
	virtual CrossTenantMessages GetOutgoingMessages(const QByteArray& sourceTenantId) const = 0;

	/**
		Get all messages associated with a specific relationship.
	*/
	virtual CrossTenantMessages GetMessagesByRelationship(const QByteArray& relationshipId) const = 0;

	/**
		Update the lifecycle status of a message.
		\param messageId The message to update.
		\param status The new status.
		\param errorMessage Optional error description (used with CTMS_FAILED).
		\return true if updated successfully.
	*/
	virtual bool UpdateMessageStatus(
				const QByteArray& messageId,
				CrossTenantMessageStatus status,
				const QString& errorMessage = QString()) = 0;

	/**
		Link a message to the local object created by the target tenant's handler.
		\return true if updated successfully.
	*/
	virtual bool SetTargetObjectId(const QByteArray& messageId, const QByteArray& targetObjectId) = 0;
};


} // namespace imtauth
