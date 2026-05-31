// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTicketServiceTest.h"


void CTicketServiceTest::init()
{
	m_servicePtr = new imtdesk::CMockTicketService();
}


void CTicketServiceTest::cleanup()
{
	delete m_servicePtr;
	m_servicePtr = nullptr;
}


// --- Create tests ---

void CTicketServiceTest::testCreateTicket_Success()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug Report", "Something broke", imtdesk::TT_BUG_REPORT, imtdesk::TP_HIGH);

	QVERIFY(!id.isEmpty());
	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket != nullptr);
	QCOMPARE(ticket->title, QString("Bug Report"));
	QCOMPARE(ticket->description, QString("Something broke"));
	QCOMPARE(ticket->ticketType, imtdesk::TT_BUG_REPORT);
	QCOMPARE(ticket->priority, imtdesk::TP_HIGH);
	QCOMPARE(ticket->status, imtdesk::TS_OPEN);
	QCOMPARE(ticket->stateReason, imtdesk::SR_NONE);
	QVERIFY(!ticket->locked);
	QVERIFY(!ticket->createdAt.isEmpty());
}


void CTicketServiceTest::testCreateTicket_EmptyTitle_Fails()
{
	QByteArray id = m_servicePtr->CreateTicket("", "desc", imtdesk::TT_SUPPORT_REQUEST, imtdesk::TP_LOW);
	QVERIFY(id.isEmpty());
	QCOMPARE(m_servicePtr->m_tickets.size(), 0);
}


void CTicketServiceTest::testCreateTicket_AllFieldsSet()
{
	QByteArrayList assignees = {"user1", "user2"};
	QByteArray convId = "conv-123";
	QByteArray msgId = "msg-456";

	QByteArray id = m_servicePtr->CreateTicket("Access Request", "Need access",
		imtdesk::TT_ACCESS_REQUEST, imtdesk::TP_CRITICAL, assignees, convId, msgId);

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->assigneeIds, assignees);
	QCOMPARE(ticket->conversationId, convId);
	QCOMPARE(ticket->messageId, msgId);
	QCOMPARE(ticket->ticketType, imtdesk::TT_ACCESS_REQUEST);
	QCOMPARE(ticket->priority, imtdesk::TP_CRITICAL);
}


void CTicketServiceTest::testCreateTicket_AutoIncrementNumber()
{
	QByteArray id1 = m_servicePtr->CreateTicket("T1", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);
	QByteArray id2 = m_servicePtr->CreateTicket("T2", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);
	QByteArray id3 = m_servicePtr->CreateTicket("T3", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	QCOMPARE(m_servicePtr->GetTicket(id1)->number, 1);
	QCOMPARE(m_servicePtr->GetTicket(id2)->number, 2);
	QCOMPARE(m_servicePtr->GetTicket(id3)->number, 3);
}


// --- Get tests ---

void CTicketServiceTest::testGetTicket_Existing()
{
	QByteArray id = m_servicePtr->CreateTicket("Existing", "", imtdesk::TT_FEATURE_REQUEST, imtdesk::TP_MEDIUM);
	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket != nullptr);
	QCOMPARE(ticket->id, id);
}


void CTicketServiceTest::testGetTicket_NonExisting_ReturnsNull()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	auto ticket = m_servicePtr->GetTicket(fakeId);
	QVERIFY(ticket == nullptr);
}


void CTicketServiceTest::testGetTickets_NoFilter()
{
	m_servicePtr->CreateTicket("T1", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);
	m_servicePtr->CreateTicket("T2", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_MEDIUM);
	m_servicePtr->CreateTicket("T3", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_HIGH);

	QByteArrayList all = m_servicePtr->GetTickets();
	QCOMPARE(all.size(), 3);
}


void CTicketServiceTest::testGetTickets_FilterByStatus()
{
	QByteArray id1 = m_servicePtr->CreateTicket("Open1", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);
	m_servicePtr->CreateTicket("Open2", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);
	QByteArray id3 = m_servicePtr->CreateTicket("ToBeClosed", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	// Close one ticket
	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id3);
	update.status = imtdesk::TS_CLOSED;
	update.closedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
	m_servicePtr->UpdateTicket(id3, update);

	QByteArrayList openTickets = m_servicePtr->GetTickets(static_cast<int>(imtdesk::TS_OPEN));
	QCOMPARE(openTickets.size(), 2);

	QByteArrayList closedTickets = m_servicePtr->GetTickets(static_cast<int>(imtdesk::TS_CLOSED));
	QCOMPARE(closedTickets.size(), 1);
	QCOMPARE(closedTickets[0], id3);
}


void CTicketServiceTest::testGetTickets_Pagination()
{
	for (int i = 0; i < 10; ++i){
		m_servicePtr->CreateTicket(QString("Ticket %1").arg(i), "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);
	}

	// Get first 3
	QByteArrayList page1 = m_servicePtr->GetTickets(-1, 0, 3);
	QCOMPARE(page1.size(), 3);

	// Get next 3
	QByteArrayList page2 = m_servicePtr->GetTickets(-1, 3, 3);
	QCOMPARE(page2.size(), 3);

	// No overlap
	for (const QByteArray& id : page1){
		QVERIFY(!page2.contains(id));
	}

	// Offset beyond total
	QByteArrayList empty = m_servicePtr->GetTickets(-1, 100, 3);
	QVERIFY(empty.isEmpty());
}


void CTicketServiceTest::testGetTickets_EmptyCollection()
{
	QByteArrayList result = m_servicePtr->GetTickets();
	QVERIFY(result.isEmpty());
}


// --- Update tests ---

void CTicketServiceTest::testUpdateTicket_Success()
{
	QByteArray id = m_servicePtr->CreateTicket("Original", "desc1", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id);
	update.title = "Updated Title";
	update.description = "Updated desc";
	QVERIFY(m_servicePtr->UpdateTicket(id, update));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->title, QString("Updated Title"));
	QCOMPARE(ticket->description, QString("Updated desc"));
}


void CTicketServiceTest::testUpdateTicket_NonExisting_ReturnsFalse()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	imtdesk::MockTicketData update;
	update.title = "test";
	QVERIFY(!m_servicePtr->UpdateTicket(fakeId, update));
}


void CTicketServiceTest::testUpdateTicket_ChangeStatus()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_HIGH);

	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id);
	update.status = imtdesk::TS_CLOSED;
	update.stateReason = imtdesk::SR_COMPLETED;
	QVERIFY(m_servicePtr->UpdateTicket(id, update));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->status, imtdesk::TS_CLOSED);
	QCOMPARE(ticket->stateReason, imtdesk::SR_COMPLETED);
}


void CTicketServiceTest::testUpdateTicket_ChangePriority()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id);
	update.priority = imtdesk::TP_CRITICAL;
	QVERIFY(m_servicePtr->UpdateTicket(id, update));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->priority, imtdesk::TP_CRITICAL);
}


void CTicketServiceTest::testUpdateTicket_AddAssignees()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id);
	update.assigneeIds = {"user1", "user2", "user3"};
	QVERIFY(m_servicePtr->UpdateTicket(id, update));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->assigneeIds.size(), 3);
	QVERIFY(ticket->assigneeIds.contains("user1"));
	QVERIFY(ticket->assigneeIds.contains("user2"));
	QVERIFY(ticket->assigneeIds.contains("user3"));
}


void CTicketServiceTest::testUpdateTicket_LockTicket()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id);
	update.locked = true;
	update.lockReason = "resolved";
	QVERIFY(m_servicePtr->UpdateTicket(id, update));

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket->locked);
	QCOMPARE(ticket->lockReason, QString("resolved"));
}


void CTicketServiceTest::testUpdateTicket_SetClosedAt()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", "", imtdesk::TT_BUG_REPORT, imtdesk::TP_LOW);

	imtdesk::MockTicketData update = *m_servicePtr->GetTicket(id);
	update.status = imtdesk::TS_CLOSED;
	update.closedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
	QVERIFY(m_servicePtr->UpdateTicket(id, update));

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(!ticket->closedAt.isEmpty());
}
