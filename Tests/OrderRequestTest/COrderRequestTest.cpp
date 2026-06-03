// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "COrderRequestTest.h"

// ImtCore includes
#include <imtauth/COrderRequest.h>


namespace
{


imtauth::OrderRequestInfo MakeSampleInfo()
{
	imtauth::OrderRequestInfo info;
	info.orderRequestId = "orderRequest-1";
	info.messageId = "msg-1";
	info.sourceTenantId = "tenantB";
	info.targetTenantId = "tenantA";
	info.relationshipId = "rel-1";
	info.sourceOrderId = "order-42";
	info.articleNumber = "A-1";
	info.quantity = 5;
	info.note = "please deliver soon";
	info.status = imtauth::ORS_CONFIRMED;
	info.statusNote = "accepted";
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.updatedAt = "2026-01-01T00:01:00.000Z";
	return info;
}


imtauth::CrossTenantMessageInfo MakeOrderRequestMessage()
{
	imtauth::CrossTenantMessageInfo info;
	info.messageId = "msg-7";
	info.sourceTenantId = "tenantB";
	info.targetTenantId = "tenantA";
	info.relationshipId = "rel-1";
	info.sourceObjectId = "order-42";
	info.messageType = imtauth::CTMT_ORDER_REQUEST;
	info.payload = "{\"articleNumber\":\"A-1\",\"quantity\":5,\"note\":\"urgent\"}";
	return info;
}


} // anonymous namespace


void COrderRequestTest::init()
{
	m_handlerPtr = new imtauth::CMockOrderRequestHandler();
	m_brokerPtr = new imtauth::CMockFeedbackBroker();
	m_handlerPtr->m_brokerPtr = m_brokerPtr;
}


void COrderRequestTest::cleanup()
{
	delete m_handlerPtr;
	m_handlerPtr = nullptr;
	delete m_brokerPtr;
	m_brokerPtr = nullptr;
}


void COrderRequestTest::testOrderRequestInfo_RoundTrip()
{
	imtauth::COrderRequest orderRequest;
	imtauth::OrderRequestInfo info = MakeSampleInfo();
	orderRequest.SetOrderRequestInfo(info);

	QCOMPARE(orderRequest.GetOrderRequestId(), QByteArray("orderRequest-1"));
	QVERIFY(orderRequest.GetOrderRequestInfo() == info);
}


void COrderRequestTest::testOrderRequestInfo_CopyFromAndClone()
{
	imtauth::COrderRequest source;
	source.SetOrderRequestInfo(MakeSampleInfo());

	imtauth::COrderRequest target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.GetOrderRequestInfo() == source.GetOrderRequestInfo());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	const imtauth::IOrderRequestData* clonedPtr = dynamic_cast<const imtauth::IOrderRequestData*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->GetOrderRequestInfo() == source.GetOrderRequestInfo());
}


void COrderRequestTest::testOrderRequestInfo_ResetDataDefaults()
{
	imtauth::COrderRequest orderRequest;
	orderRequest.SetOrderRequestInfo(MakeSampleInfo());

	QVERIFY(orderRequest.ResetData());

	imtauth::OrderRequestInfo info = orderRequest.GetOrderRequestInfo();
	QVERIFY(info.orderRequestId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.targetTenantId.isEmpty());
	QCOMPARE(info.quantity, 0);
	QCOMPARE(info.status, imtauth::ORS_RECEIVED);
}


void COrderRequestTest::testHandleMessage_MaterializesOrderRequest()
{
	QByteArray targetObjectId;
	QVERIFY(m_handlerPtr->HandleMessage(MakeOrderRequestMessage(), targetObjectId));
	QVERIFY(!targetObjectId.isEmpty());

	imtauth::OrderRequestInfo materialized = m_handlerPtr->GetOrderRequest(targetObjectId);
	QCOMPARE(materialized.sourceTenantId, QByteArray("tenantB"));
	QCOMPARE(materialized.targetTenantId, QByteArray("tenantA"));
	QCOMPARE(materialized.relationshipId, QByteArray("rel-1"));
	QCOMPARE(materialized.sourceOrderId, QByteArray("order-42"));
	QCOMPARE(materialized.articleNumber, QString("A-1"));
	QCOMPARE(materialized.quantity, 5);
	QCOMPARE(materialized.note, QString("urgent"));
	QCOMPARE(materialized.status, imtauth::ORS_RECEIVED);
}


void COrderRequestTest::testHandleMessage_OnlyHandlesOrderRequestType()
{
	QVERIFY(m_handlerPtr->CanHandle(imtauth::CTMT_ORDER_REQUEST));
	QVERIFY(!m_handlerPtr->CanHandle(imtauth::CTMT_ORDER_CONFIRMATION));
	QVERIFY(!m_handlerPtr->CanHandle(imtauth::CTMT_DOCUMENT_SHARE));
	QVERIFY(!m_handlerPtr->CanHandle(imtauth::CTMT_CUSTOM));
}


void COrderRequestTest::testConfirmOrderRequest_SendsConfirmationFeedback()
{
	QByteArray targetObjectId;
	QVERIFY(m_handlerPtr->HandleMessage(MakeOrderRequestMessage(), targetObjectId));

	QVERIFY(m_handlerPtr->ConfirmOrderRequest(targetObjectId, "in stock"));
	QCOMPARE(m_handlerPtr->GetOrderRequest(targetObjectId).status, imtauth::ORS_CONFIRMED);

	QCOMPARE(m_brokerPtr->m_messages.size(), 1);
	const imtauth::CrossTenantMessageInfo& feedback = m_brokerPtr->m_messages.first();
	QCOMPARE(feedback.messageType, imtauth::CTMT_ORDER_CONFIRMATION);
	// Feedback travels back in the reverse direction.
	QCOMPARE(feedback.sourceTenantId, QByteArray("tenantA"));
	QCOMPARE(feedback.targetTenantId, QByteArray("tenantB"));
	QCOMPARE(feedback.relationshipId, QByteArray("rel-1"));

	QJsonObject payloadObject = QJsonDocument::fromJson(feedback.payload).object();
	QCOMPARE(payloadObject.value("sourceOrderId").toString(), QString("order-42"));
	QCOMPARE(payloadObject.value("status").toInt(), static_cast<int>(imtauth::ORS_CONFIRMED));
	QCOMPARE(payloadObject.value("note").toString(), QString("in stock"));
}


void COrderRequestTest::testRejectOrderRequest_SendsRejectionFeedback()
{
	QByteArray targetObjectId;
	QVERIFY(m_handlerPtr->HandleMessage(MakeOrderRequestMessage(), targetObjectId));

	QVERIFY(m_handlerPtr->RejectOrderRequest(targetObjectId, "out of stock"));
	QCOMPARE(m_handlerPtr->GetOrderRequest(targetObjectId).status, imtauth::ORS_REJECTED);

	QCOMPARE(m_brokerPtr->m_messages.size(), 1);
	QCOMPARE(m_brokerPtr->m_messages.first().messageType, imtauth::CTMT_ORDER_REJECTION);
}


void COrderRequestTest::testUpdateStatus_SendsStatusUpdateFeedback()
{
	QByteArray targetObjectId;
	QVERIFY(m_handlerPtr->HandleMessage(MakeOrderRequestMessage(), targetObjectId));

	QVERIFY(m_handlerPtr->UpdateOrderRequestStatus(targetObjectId, imtauth::ORS_IN_PROGRESS, "processing"));
	QCOMPARE(m_handlerPtr->GetOrderRequest(targetObjectId).status, imtauth::ORS_IN_PROGRESS);

	QCOMPARE(m_brokerPtr->m_messages.size(), 1);
	QCOMPARE(m_brokerPtr->m_messages.first().messageType, imtauth::CTMT_ORDER_STATUS_UPDATE);
}


void COrderRequestTest::testStatusFeedback_UnknownRequest_Fails()
{
	QVERIFY(!m_handlerPtr->ConfirmOrderRequest("does-not-exist"));
	QCOMPARE(m_brokerPtr->m_messages.size(), 0);
}
