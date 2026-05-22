// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTicketLifecycleTest.h"


void CTicketLifecycleTest::init()
{
	m_servicePtr = new lifecycle::CLifecycleTicketService();
	m_reporterId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
}


void CTicketLifecycleTest::cleanup()
{
	delete m_servicePtr;
	m_servicePtr = nullptr;
}


// --- State transitions ---

void CTicketLifecycleTest::testTicketCreation_InitialState()
{
	QByteArray id = m_servicePtr->CreateTicket("New Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket != nullptr);
	QCOMPARE(ticket->status, lifecycle::TS_OPEN);
	QCOMPARE(ticket->stateReason, lifecycle::SR_NONE);
	QVERIFY(!ticket->locked);
	QVERIFY(ticket->assigneeIds.isEmpty());
	QVERIFY(ticket->createdAt.isValid());
	QCOMPARE(ticket->reporterId, m_reporterId);
	QVERIFY(!ticket->closedAt.isValid());
	QVERIFY(!ticket->resolvedAt.isValid());
}


void CTicketLifecycleTest::testTicketOpen_ToClose_Completed()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_HIGH, m_reporterId);

	QVERIFY(m_servicePtr->CloseTicket(id, lifecycle::SR_COMPLETED));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->status, lifecycle::TS_CLOSED);
	QCOMPARE(ticket->stateReason, lifecycle::SR_COMPLETED);
	QVERIFY(ticket->closedAt.isValid());
	QVERIFY(ticket->resolvedAt.isValid()); // Completed = resolved
}


void CTicketLifecycleTest::testTicketOpen_ToClose_NotPlanned()
{
	QByteArray id = m_servicePtr->CreateTicket("Feature", lifecycle::TT_FEATURE_REQUEST,
		lifecycle::TP_LOW, m_reporterId);

	QVERIFY(m_servicePtr->CloseTicket(id, lifecycle::SR_NOT_PLANNED));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->status, lifecycle::TS_CLOSED);
	QCOMPARE(ticket->stateReason, lifecycle::SR_NOT_PLANNED);
	QVERIFY(ticket->closedAt.isValid());
	QVERIFY(!ticket->resolvedAt.isValid()); // Not planned ≠ resolved
}


void CTicketLifecycleTest::testTicketClosed_Reopen()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_HIGH, m_reporterId);
	m_servicePtr->CloseTicket(id, lifecycle::SR_COMPLETED);

	QVERIFY(m_servicePtr->ReopenTicket(id));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->status, lifecycle::TS_OPEN);
	QCOMPARE(ticket->stateReason, lifecycle::SR_REOPENED);
	QVERIFY(!ticket->closedAt.isValid()); // Cleared on reopen
}


void CTicketLifecycleTest::testTicketReopen_ToClose()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_HIGH, m_reporterId);
	m_servicePtr->CloseTicket(id, lifecycle::SR_COMPLETED);
	m_servicePtr->ReopenTicket(id);

	QVERIFY(m_servicePtr->CloseTicket(id, lifecycle::SR_COMPLETED));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->status, lifecycle::TS_CLOSED);
	QVERIFY(ticket->closedAt.isValid());
}


// --- Priority ---

void CTicketLifecycleTest::testTicketPriorityEscalation()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_LOW, m_reporterId);

	QVERIFY(m_servicePtr->SetPriority(id, lifecycle::TP_CRITICAL));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->priority, lifecycle::TP_CRITICAL);
}


void CTicketLifecycleTest::testTicketPriorityDeescalation()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_CRITICAL, m_reporterId);

	QVERIFY(m_servicePtr->SetPriority(id, lifecycle::TP_LOW));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->priority, lifecycle::TP_LOW);
}


// --- Locking ---

void CTicketLifecycleTest::testTicketLock_PreventsUpdates()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	QVERIFY(m_servicePtr->LockTicket(id, "resolved"));

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket->locked);
	QCOMPARE(ticket->lockReason, QString("resolved"));

	// Assignment should fail when locked
	QVERIFY(!m_servicePtr->SetAssignees(id, {"user1"}));
}


void CTicketLifecycleTest::testTicketUnlock_AllowsUpdates()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);
	m_servicePtr->LockTicket(id, "resolved");

	QVERIFY(m_servicePtr->UnlockTicket(id));

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(!ticket->locked);
	QVERIFY(ticket->lockReason.isEmpty());

	// Now assignment should succeed
	QVERIFY(m_servicePtr->SetAssignees(id, {"user1"}));
}


// --- Assignment ---

void CTicketLifecycleTest::testTicketAssignment_Single()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	QVERIFY(m_servicePtr->SetAssignees(id, {"user-1"}));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->assigneeIds.size(), 1);
	QCOMPARE(ticket->assigneeIds[0], QByteArray("user-1"));
}


void CTicketLifecycleTest::testTicketAssignment_Multiple()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	QByteArrayList assignees = {"user-1", "user-2", "user-3"};
	QVERIFY(m_servicePtr->SetAssignees(id, assignees));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->assigneeIds.size(), 3);
}


void CTicketLifecycleTest::testTicketAssignment_Reassign()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	m_servicePtr->SetAssignees(id, {"user-1"});
	QVERIFY(m_servicePtr->SetAssignees(id, {"user-2"}));

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->assigneeIds.size(), 1);
	QCOMPARE(ticket->assigneeIds[0], QByteArray("user-2"));
}


void CTicketLifecycleTest::testTicketAssignment_Unassign()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	m_servicePtr->SetAssignees(id, {"user-1", "user-2"});
	QVERIFY(m_servicePtr->SetAssignees(id, {}));

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket->assigneeIds.isEmpty());
}


// --- Labels ---

void CTicketLifecycleTest::testTicketLabels_AddRemove()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	// Add labels
	QVERIFY(m_servicePtr->SetLabels(id, {"label-1", "label-2", "label-3"}));
	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->labelIds.size(), 3);

	// Replace with fewer
	QVERIFY(m_servicePtr->SetLabels(id, {"label-1"}));
	ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->labelIds.size(), 1);

	// Clear all
	QVERIFY(m_servicePtr->SetLabels(id, {}));
	ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket->labelIds.isEmpty());
}


// --- Timestamps ---

void CTicketLifecycleTest::testTicketTimestamps_CreatedAtImmutable()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	QDateTime originalCreatedAt = m_servicePtr->GetTicket(id)->createdAt;

	QTest::qWait(5);
	m_servicePtr->SetPriority(id, lifecycle::TP_HIGH);

	auto ticket = m_servicePtr->GetTicket(id);
	QCOMPARE(ticket->createdAt, originalCreatedAt); // Not changed
}


void CTicketLifecycleTest::testTicketTimestamps_UpdatedAtChanges()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	QDateTime originalUpdatedAt = m_servicePtr->GetTicket(id)->updatedAt;

	QTest::qWait(5);
	m_servicePtr->SetPriority(id, lifecycle::TP_HIGH);

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket->updatedAt >= originalUpdatedAt);
}


void CTicketLifecycleTest::testTicketTimestamps_ClosedAtOnClose()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	QVERIFY(!m_servicePtr->GetTicket(id)->closedAt.isValid());

	m_servicePtr->CloseTicket(id, lifecycle::SR_COMPLETED);

	auto ticket = m_servicePtr->GetTicket(id);
	QVERIFY(ticket->closedAt.isValid());

	// After reopen, closedAt should be cleared
	m_servicePtr->ReopenTicket(id);
	ticket = m_servicePtr->GetTicket(id);
	QVERIFY(!ticket->closedAt.isValid());
}


void CTicketLifecycleTest::testTicketTimestamps_ResolvedAtOnResolve()
{
	QByteArray id = m_servicePtr->CreateTicket("Bug", lifecycle::TT_BUG_REPORT,
		lifecycle::TP_MEDIUM, m_reporterId);

	// Close as completed = resolved
	m_servicePtr->CloseTicket(id, lifecycle::SR_COMPLETED);
	QVERIFY(m_servicePtr->GetTicket(id)->resolvedAt.isValid());

	// Create another and close as not planned = NOT resolved
	QByteArray id2 = m_servicePtr->CreateTicket("Feature", lifecycle::TT_FEATURE_REQUEST,
		lifecycle::TP_LOW, m_reporterId);
	m_servicePtr->CloseTicket(id2, lifecycle::SR_NOT_PLANNED);
	QVERIFY(!m_servicePtr->GetTicket(id2)->resolvedAt.isValid());
}


// --- Number ---

void CTicketLifecycleTest::testTicketNumber_Sequential()
{
	QByteArray id1 = m_servicePtr->CreateTicket("T1", lifecycle::TT_BUG_REPORT, lifecycle::TP_LOW, m_reporterId);
	QByteArray id2 = m_servicePtr->CreateTicket("T2", lifecycle::TT_BUG_REPORT, lifecycle::TP_LOW, m_reporterId);
	QByteArray id3 = m_servicePtr->CreateTicket("T3", lifecycle::TT_BUG_REPORT, lifecycle::TP_LOW, m_reporterId);
	QByteArray id4 = m_servicePtr->CreateTicket("T4", lifecycle::TT_BUG_REPORT, lifecycle::TP_LOW, m_reporterId);
	QByteArray id5 = m_servicePtr->CreateTicket("T5", lifecycle::TT_BUG_REPORT, lifecycle::TP_LOW, m_reporterId);

	QCOMPARE(m_servicePtr->GetTicket(id1)->number, 1);
	QCOMPARE(m_servicePtr->GetTicket(id2)->number, 2);
	QCOMPARE(m_servicePtr->GetTicket(id3)->number, 3);
	QCOMPARE(m_servicePtr->GetTicket(id4)->number, 4);
	QCOMPARE(m_servicePtr->GetTicket(id5)->number, 5);
}
