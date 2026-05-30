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
	Lifecycle status of an order request (RFC phase 3, §15).

	An order request is the target-tenant materialization of an order produced
	by a source tenant. The status reflects the local processing of that request
	in the receiving tenant and is mirrored back to the source tenant through
	status-feedback messages.
*/
enum OrderRequestStatus
{
	ORS_RECEIVED = 0,
	ORS_CONFIRMED,
	ORS_REJECTED,
	ORS_IN_PROGRESS,
	ORS_COMPLETED,
	ORS_CANCELLED
};


/**
	Structure describing a single order request.

	This is the concrete domain object of the first cross-tenant scenario
	(Order -> OrderRequest). The source tenant owns an order; a cross-tenant
	message materializes a brand-new, local order request inside the target
	tenant. Both tenants only ever work with their own local objects, linked
	through \a sourceOrderId (source order) and \a orderRequestId (target order
	request) and correlated through \a messageId.
*/
struct OrderRequestInfo
{
	QByteArray orderRequestId;
	QByteArray messageId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray sourceOrderId;
	QString articleNumber;
	int quantity = 0;
	QString note;
	OrderRequestStatus status = ORS_RECEIVED;
	QString statusNote;
	QString createdAt;
	QString updatedAt;

	bool operator==(const OrderRequestInfo& other) const
	{
		return orderRequestId == other.orderRequestId
			&& messageId == other.messageId
			&& sourceTenantId == other.sourceTenantId
			&& targetTenantId == other.targetTenantId
			&& relationshipId == other.relationshipId
			&& sourceOrderId == other.sourceOrderId
			&& articleNumber == other.articleNumber
			&& quantity == other.quantity
			&& note == other.note
			&& status == other.status
			&& statusNote == other.statusNote
			&& createdAt == other.createdAt
			&& updatedAt == other.updatedAt;
	}

	bool operator!=(const OrderRequestInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<OrderRequestInfo> OrderRequests;


/**
	Interface of the order-request domain (RFC phase 3, §15).

	Provides read access to the order requests materialized inside the target
	tenant and the status-feedback operations (confirm / reject / status update)
	that report the local processing result back to the originating source
	tenant. The feedback is delivered through the cross-tenant message broker as
	a new message in the reverse direction, so the source tenant can update its
	own order without ever gaining direct access to the target tenant's data.

	\ingroup Tenant
*/
class IOrderRequest: virtual public istd::IChangeable
{
public:
	/**
		Get a specific order request by its ID.
	*/
	virtual OrderRequestInfo GetOrderRequest(const QByteArray& orderRequestId) const = 0;

	/**
		Get all order requests materialized inside the specified target tenant.
	*/
	virtual OrderRequests GetOrderRequests(const QByteArray& targetTenantId) const = 0;

	/**
		Get all order requests associated with a specific relationship.
	*/
	virtual OrderRequests GetOrderRequestsByRelationship(const QByteArray& relationshipId) const = 0;

	/**
		Confirm an order request and report the confirmation back to the source
		tenant through a CTMT_ORDER_CONFIRMATION message.
		\param orderRequestId The order request to confirm.
		\param note Optional confirmation note propagated to the source tenant.
		\return true if the request was confirmed and the feedback dispatched.
	*/
	virtual bool ConfirmOrderRequest(const QByteArray& orderRequestId, const QString& note = QString()) = 0;

	/**
		Reject an order request and report the rejection back to the source
		tenant through a CTMT_ORDER_REJECTION message.
		\param orderRequestId The order request to reject.
		\param reason Optional rejection reason propagated to the source tenant.
		\return true if the request was rejected and the feedback dispatched.
	*/
	virtual bool RejectOrderRequest(const QByteArray& orderRequestId, const QString& reason = QString()) = 0;

	/**
		Update the local status of an order request and report the new status
		back to the source tenant through a CTMT_ORDER_STATUS_UPDATE message.
		\param orderRequestId The order request to update.
		\param status The new local status.
		\param note Optional status note propagated to the source tenant.
		\return true if the status was updated and the feedback dispatched.
	*/
	virtual bool UpdateOrderRequestStatus(
				const QByteArray& orderRequestId,
				OrderRequestStatus status,
				const QString& note = QString()) = 0;
};


} // namespace imtauth
