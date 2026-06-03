// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/IOrderRequest.h>
#include <imtauth/ICrossTenantMessage.h>


namespace imtauth
{


/**
	Self-contained mock broker capturing the status-feedback messages sent back
	to the source tenant by the order-request handler.
*/
class CMockFeedbackBroker
{
public:
	QByteArray SendMessage(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& relationshipId,
				CrossTenantMessageType messageType,
				const QByteArray& payload,
				const QByteArray& sourceObjectId = QByteArray())
	{
		CrossTenantMessageInfo info;
		info.messageId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.targetTenantId = targetTenantId;
		info.relationshipId = relationshipId;
		info.sourceObjectId = sourceObjectId;
		info.messageType = messageType;
		info.payload = payload;
		m_messages.append(info);
		return info.messageId;
	}

	QList<CrossTenantMessageInfo> m_messages;
	int m_counter = 0;
};


/**
	Self-contained mock replicating the semantics of COrderRequestHandlerComp:
	materialization of a local order request from an order-request message and
	the status-feedback (confirm / reject / update) operations dispatched back to
	the source tenant through the broker.
*/
class CMockOrderRequestHandler
{
public:
	bool CanHandle(CrossTenantMessageType messageType) const
	{
		return messageType == CTMT_ORDER_REQUEST;
	}

	bool HandleMessage(const CrossTenantMessageInfo& messageInfo, QByteArray& targetObjectId)
	{
		QJsonObject payloadObject = QJsonDocument::fromJson(messageInfo.payload).object();

		OrderRequestInfo info;
		info.orderRequestId = QByteArray("orderRequest-") + QByteArray::number(++m_counter);
		info.messageId = messageInfo.messageId;
		info.sourceTenantId = messageInfo.sourceTenantId;
		info.targetTenantId = messageInfo.targetTenantId;
		info.relationshipId = messageInfo.relationshipId;
		info.sourceOrderId = messageInfo.sourceObjectId;
		info.articleNumber = payloadObject.value("articleNumber").toString();
		info.quantity = payloadObject.value("quantity").toInt();
		info.note = payloadObject.value("note").toString();
		info.status = ORS_RECEIVED;

		m_store.insert(info.orderRequestId, info);
		targetObjectId = info.orderRequestId;
		return true;
	}

	bool ApplyStatusFeedback(
				const QByteArray& orderRequestId,
				OrderRequestStatus status,
				CrossTenantMessageType feedbackType,
				const QString& note)
	{
		if (!m_store.contains(orderRequestId)){
			return false;
		}

		OrderRequestInfo& info = m_store[orderRequestId];
		info.status = status;
		info.statusNote = note;

		if (m_brokerPtr != nullptr){
			QJsonObject feedbackObject;
			feedbackObject.insert("orderRequestId", QString::fromUtf8(info.orderRequestId));
			feedbackObject.insert("sourceOrderId", QString::fromUtf8(info.sourceOrderId));
			feedbackObject.insert("status", static_cast<int>(status));
			feedbackObject.insert("note", note);

			m_brokerPtr->SendMessage(
						info.targetTenantId,
						info.sourceTenantId,
						info.relationshipId,
						feedbackType,
						QJsonDocument(feedbackObject).toJson(QJsonDocument::Compact),
						info.orderRequestId);
		}

		return true;
	}

	bool ConfirmOrderRequest(const QByteArray& orderRequestId, const QString& note = QString())
	{
		return ApplyStatusFeedback(orderRequestId, ORS_CONFIRMED, CTMT_ORDER_CONFIRMATION, note);
	}

	bool RejectOrderRequest(const QByteArray& orderRequestId, const QString& reason = QString())
	{
		return ApplyStatusFeedback(orderRequestId, ORS_REJECTED, CTMT_ORDER_REJECTION, reason);
	}

	bool UpdateOrderRequestStatus(const QByteArray& orderRequestId, OrderRequestStatus status, const QString& note = QString())
	{
		return ApplyStatusFeedback(orderRequestId, status, CTMT_ORDER_STATUS_UPDATE, note);
	}

	OrderRequestInfo GetOrderRequest(const QByteArray& orderRequestId) const
	{
		return m_store.value(orderRequestId);
	}

	QHash<QByteArray, OrderRequestInfo> m_store;
	CMockFeedbackBroker* m_brokerPtr = nullptr;
	int m_counter = 0;
};


} // namespace imtauth


class COrderRequestTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testOrderRequestInfo_RoundTrip();
	void testOrderRequestInfo_CopyFromAndClone();
	void testOrderRequestInfo_ResetDataDefaults();

	// Handler semantics
	void testHandleMessage_MaterializesOrderRequest();
	void testHandleMessage_OnlyHandlesOrderRequestType();
	void testConfirmOrderRequest_SendsConfirmationFeedback();
	void testRejectOrderRequest_SendsRejectionFeedback();
	void testUpdateStatus_SendsStatusUpdateFeedback();
	void testStatusFeedback_UnknownRequest_Fails();

private:
	imtauth::CMockOrderRequestHandler* m_handlerPtr = nullptr;
	imtauth::CMockFeedbackBroker* m_brokerPtr = nullptr;
};
