// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCrossTenantMessageTest.h"

// ImtCore includes
#include <imtauth/CCrossTenantMessage.h>


namespace
{


imtauth::CrossTenantMessageInfo MakeSampleInfo()
{
	imtauth::CrossTenantMessageInfo info;
	info.messageId = "msg-1";
	info.sourceTenantId = "tenantB";
	info.targetTenantId = "tenantA";
	info.relationshipId = "rel-1";
	info.sourceObjectId = "order-42";
	info.targetObjectId = "orderRequest-7";
	info.messageType = imtauth::CTMT_ORDER_REQUEST;
	info.customType = QString();
	info.payload = "{\"articleNumber\":\"A-1\"}";
	info.status = imtauth::CTMS_PROCESSED;
	info.errorMessage = QString();
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.updatedAt = "2026-01-01T00:01:00.000Z";
	info.expiresAt = "2030-01-01T00:00:00.000Z";
	return info;
}


} // anonymous namespace


void CCrossTenantMessageTest::init()
{
	m_brokerPtr = new imtauth::CMockMessageBroker();
	m_projectionPtr = new imtauth::CMockMessageProjection();
	m_handlerPtr = new imtauth::CMockOrderHandler();
}


void CCrossTenantMessageTest::cleanup()
{
	delete m_brokerPtr;
	m_brokerPtr = nullptr;
	delete m_projectionPtr;
	m_projectionPtr = nullptr;
	delete m_handlerPtr;
	m_handlerPtr = nullptr;
}


void CCrossTenantMessageTest::testMessageInfo_RoundTrip()
{
	imtauth::CCrossTenantMessage message;
	imtauth::CrossTenantMessageInfo info = MakeSampleInfo();
	message.SetMessageInfo(info);

	QCOMPARE(message.GetMessageId(), QByteArray("msg-1"));
	QVERIFY(message.GetMessageInfo() == info);
}


void CCrossTenantMessageTest::testMessageInfo_CopyFromAndClone()
{
	imtauth::CCrossTenantMessage source;
	source.SetMessageInfo(MakeSampleInfo());

	imtauth::CCrossTenantMessage target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.GetMessageInfo() == source.GetMessageInfo());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	const imtauth::ICrossTenantMessageData* clonedPtr = dynamic_cast<const imtauth::ICrossTenantMessageData*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->GetMessageInfo() == source.GetMessageInfo());
}


void CCrossTenantMessageTest::testMessageInfo_ResetDataDefaults()
{
	imtauth::CCrossTenantMessage message;
	message.SetMessageInfo(MakeSampleInfo());

	QVERIFY(message.ResetData());

	imtauth::CrossTenantMessageInfo info = message.GetMessageInfo();
	QVERIFY(info.messageId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.targetTenantId.isEmpty());
	QCOMPARE(info.messageType, imtauth::CTMT_CUSTOM);
	QCOMPARE(info.status, imtauth::CTMS_CREATED);
}


void CCrossTenantMessageTest::testProjection_PassThroughWhenUnconfigured()
{
	const QByteArray payload = "{\"internalPrice\":100,\"articleNumber\":\"A-1\"}";
	bool isValid = false;
	QByteArray projected = m_projectionPtr->ProjectPayload(imtauth::CTMT_ORDER_REQUEST, QString(), payload, isValid);
	QVERIFY(isValid);
	QCOMPARE(projected, payload);
}


void CCrossTenantMessageTest::testProjection_WhitelistKeepsAllowedFields()
{
	m_projectionPtr->m_allowedFields = QStringList() << "articleNumber" << "quantity";

	const QByteArray payload = "{\"articleNumber\":\"A-1\",\"quantity\":5,\"internalPrice\":100,\"costCenter\":\"CC-9\"}";
	bool isValid = false;
	QByteArray projected = m_projectionPtr->ProjectPayload(imtauth::CTMT_ORDER_REQUEST, QString(), payload, isValid);
	QVERIFY(isValid);

	QJsonObject object = QJsonDocument::fromJson(projected).object();
	QVERIFY(object.contains("articleNumber"));
	QVERIFY(object.contains("quantity"));
	QVERIFY(!object.contains("internalPrice"));
	QVERIFY(!object.contains("costCenter"));
}


void CCrossTenantMessageTest::testProjection_BlacklistRemovesBlockedFields()
{
	m_projectionPtr->m_blockedFields = QSet<QString>() << "internalPrice" << "budgetCode";

	const QByteArray payload = "{\"articleNumber\":\"A-1\",\"internalPrice\":100,\"budgetCode\":\"B-1\"}";
	bool isValid = false;
	QByteArray projected = m_projectionPtr->ProjectPayload(imtauth::CTMT_ORDER_REQUEST, QString(), payload, isValid);
	QVERIFY(isValid);

	QJsonObject object = QJsonDocument::fromJson(projected).object();
	QVERIFY(object.contains("articleNumber"));
	QVERIFY(!object.contains("internalPrice"));
	QVERIFY(!object.contains("budgetCode"));
}


void CCrossTenantMessageTest::testSendMessage_RequiresRelationship()
{
	// No relationship registered: the message must be rejected.
	QByteArray messageId = m_brokerPtr->SendMessage(
				"tenantB", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}");
	QVERIFY(messageId.isEmpty());

	m_brokerPtr->RegisterRelationship("tenantB", "tenantA", "rel-1");
	messageId = m_brokerPtr->SendMessage(
				"tenantB", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}");
	QVERIFY(!messageId.isEmpty());
}


void CCrossTenantMessageTest::testSendMessage_MissingFields_Fails()
{
	m_brokerPtr->RegisterRelationship("tenantB", "tenantA", "rel-1");
	QVERIFY(m_brokerPtr->SendMessage("", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}").isEmpty());
	QVERIFY(m_brokerPtr->SendMessage("tenantB", "", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}").isEmpty());
	QVERIFY(m_brokerPtr->SendMessage("tenantB", "tenantA", "", imtauth::CTMT_ORDER_REQUEST, "{}").isEmpty());
}


void CCrossTenantMessageTest::testSendMessage_SameTenant_Fails()
{
	m_brokerPtr->RegisterRelationship("tenantA", "tenantA", "rel-1");
	QVERIFY(m_brokerPtr->SendMessage("tenantA", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}").isEmpty());
}


void CCrossTenantMessageTest::testSendMessage_AppliesProjection()
{
	m_brokerPtr->m_projectionPtr = m_projectionPtr;
	m_projectionPtr->m_allowedFields = QStringList() << "articleNumber";
	m_brokerPtr->RegisterRelationship("tenantB", "tenantA", "rel-1");

	const QByteArray payload = "{\"articleNumber\":\"A-1\",\"internalPrice\":100}";
	QByteArray messageId = m_brokerPtr->SendMessage(
				"tenantB", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, payload);
	QVERIFY(!messageId.isEmpty());

	imtauth::CrossTenantMessageInfo stored = m_brokerPtr->GetMessage(messageId);
	QJsonObject object = QJsonDocument::fromJson(stored.payload).object();
	QVERIFY(object.contains("articleNumber"));
	QVERIFY(!object.contains("internalPrice"));
}


void CCrossTenantMessageTest::testSendMessage_DispatchesToHandler()
{
	m_brokerPtr->m_handlerPtr = m_handlerPtr;
	m_brokerPtr->RegisterRelationship("tenantB", "tenantA", "rel-1");

	QByteArray messageId = m_brokerPtr->SendMessage(
				"tenantB", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}", "order-42");
	QVERIFY(!messageId.isEmpty());
	QCOMPARE(m_handlerPtr->m_handledCount, 1);

	imtauth::CrossTenantMessageInfo stored = m_brokerPtr->GetMessage(messageId);
	QCOMPARE(stored.status, imtauth::CTMS_PROCESSED);
	QVERIFY(!stored.targetObjectId.isEmpty());

	// A non-order message is not handled and stays validated.
	QByteArray docMessageId = m_brokerPtr->SendMessage(
				"tenantB", "tenantA", "rel-1", imtauth::CTMT_DOCUMENT_SHARE, "{}");
	QVERIFY(!docMessageId.isEmpty());
	QCOMPARE(m_handlerPtr->m_handledCount, 1);
	QCOMPARE(m_brokerPtr->GetMessage(docMessageId).status, imtauth::CTMS_VALIDATED);
}


void CCrossTenantMessageTest::testSendMessage_InboxOutbox()
{
	m_brokerPtr->RegisterRelationship("tenantB", "tenantA", "rel-1");
	m_brokerPtr->SendMessage("tenantB", "tenantA", "rel-1", imtauth::CTMT_ORDER_REQUEST, "{}");

	QCOMPARE(m_brokerPtr->GetOutgoingMessages("tenantB").size(), 1);
	QCOMPARE(m_brokerPtr->GetIncomingMessages("tenantA").size(), 1);
	QCOMPARE(m_brokerPtr->GetIncomingMessages("tenantB").size(), 0);
}
