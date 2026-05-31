// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTicketPermissionsTest.h"


void CTicketPermissionsTest::init()
{
	m_currentUserId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_reporterId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	m_userInfoPtr = new imtdeskgql::MockUserInfo();
	m_userInfoPtr->userId = m_currentUserId;
	m_userInfoPtr->isAdmin = false;

	m_contextPtr = new imtdeskgql::MockContext();
	m_contextPtr->userInfoPtr = m_userInfoPtr;

	m_ticketPtr = new imtdeskgql::MockTicket();
	m_ticketPtr->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_ticketPtr->title = "Test Ticket";
	m_ticketPtr->reporterId = m_reporterId;

	m_reporterGroupsPtr = new imtdeskgql::MockReporterGroups();
}


void CTicketPermissionsTest::cleanup()
{
	delete m_reporterGroupsPtr;
	delete m_ticketPtr;
	delete m_contextPtr;
	delete m_userInfoPtr;
	m_reporterGroupsPtr = nullptr;
	m_ticketPtr = nullptr;
	m_contextPtr = nullptr;
	m_userInfoPtr = nullptr;
}


// --- IsCurrentUserAdmin ---

void CTicketPermissionsTest::testIsCurrentUserAdmin_True()
{
	m_userInfoPtr->isAdmin = true;
	QVERIFY(imtdeskgql::IsCurrentUserAdmin(m_contextPtr));
}

void CTicketPermissionsTest::testIsCurrentUserAdmin_False()
{
	m_userInfoPtr->isAdmin = false;
	QVERIFY(!imtdeskgql::IsCurrentUserAdmin(m_contextPtr));
}

void CTicketPermissionsTest::testIsCurrentUserAdmin_NullContext()
{
	QVERIFY(!imtdeskgql::IsCurrentUserAdmin(nullptr));
}


// --- IsCurrentUserRelatedToTicket ---

void CTicketPermissionsTest::testIsCurrentUserRelatedToTicket_Reporter()
{
	m_ticketPtr->reporterId = m_currentUserId;
	bool isReporter = false, isAssignee = false;
	QVERIFY(imtdeskgql::IsCurrentUserRelatedToTicket(m_contextPtr, m_ticketPtr, isReporter, isAssignee));
	QVERIFY(isReporter);
	QVERIFY(!isAssignee);
}

void CTicketPermissionsTest::testIsCurrentUserRelatedToTicket_Assignee()
{
	m_ticketPtr->assigneeIds.append(m_currentUserId);
	bool isReporter = false, isAssignee = false;
	QVERIFY(imtdeskgql::IsCurrentUserRelatedToTicket(m_contextPtr, m_ticketPtr, isReporter, isAssignee));
	QVERIFY(!isReporter);
	QVERIFY(isAssignee);
}

void CTicketPermissionsTest::testIsCurrentUserRelatedToTicket_Both()
{
	m_ticketPtr->reporterId = m_currentUserId;
	m_ticketPtr->assigneeIds.append(m_currentUserId);
	bool isReporter = false, isAssignee = false;
	QVERIFY(imtdeskgql::IsCurrentUserRelatedToTicket(m_contextPtr, m_ticketPtr, isReporter, isAssignee));
	QVERIFY(isReporter);
	QVERIFY(isAssignee);
}

void CTicketPermissionsTest::testIsCurrentUserRelatedToTicket_Unrelated()
{
	bool isReporter = false, isAssignee = false;
	QVERIFY(!imtdeskgql::IsCurrentUserRelatedToTicket(m_contextPtr, m_ticketPtr, isReporter, isAssignee));
	QVERIFY(!isReporter);
	QVERIFY(!isAssignee);
}

void CTicketPermissionsTest::testIsCurrentUserRelatedToTicket_NullContext()
{
	bool isReporter = false, isAssignee = false;
	QVERIFY(!imtdeskgql::IsCurrentUserRelatedToTicket(nullptr, m_ticketPtr, isReporter, isAssignee));
}

void CTicketPermissionsTest::testIsCurrentUserRelatedToTicket_NullTicket()
{
	bool isReporter = false, isAssignee = false;
	QVERIFY(!imtdeskgql::IsCurrentUserRelatedToTicket(m_contextPtr, nullptr, isReporter, isAssignee));
}


// --- IsCurrentUserInSameGroupAsReporter ---

void CTicketPermissionsTest::testIsCurrentUserInSameGroupAsReporter_SharedGroup()
{
	m_userInfoPtr->groups = {"group-a", "group-b"};
	m_reporterGroupsPtr->groups = {"group-b", "group-c"};
	QVERIFY(imtdeskgql::IsCurrentUserInSameGroupAsReporter(m_contextPtr, m_reporterGroupsPtr));
}

void CTicketPermissionsTest::testIsCurrentUserInSameGroupAsReporter_NoSharedGroup()
{
	m_userInfoPtr->groups = {"group-a", "group-b"};
	m_reporterGroupsPtr->groups = {"group-c", "group-d"};
	QVERIFY(!imtdeskgql::IsCurrentUserInSameGroupAsReporter(m_contextPtr, m_reporterGroupsPtr));
}

void CTicketPermissionsTest::testIsCurrentUserInSameGroupAsReporter_EmptyGroups()
{
	m_userInfoPtr->groups.clear();
	m_reporterGroupsPtr->groups = {"group-a"};
	QVERIFY(!imtdeskgql::IsCurrentUserInSameGroupAsReporter(m_contextPtr, m_reporterGroupsPtr));
}


// --- HasTicketVisibility ---

void CTicketPermissionsTest::testHasTicketVisibility_Admin()
{
	m_userInfoPtr->isAdmin = true;
	QVERIFY(imtdeskgql::HasTicketVisibility(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr));
}

void CTicketPermissionsTest::testHasTicketVisibility_Reporter()
{
	m_ticketPtr->reporterId = m_currentUserId;
	QVERIFY(imtdeskgql::HasTicketVisibility(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr));
}

void CTicketPermissionsTest::testHasTicketVisibility_Assignee()
{
	m_ticketPtr->assigneeIds.append(m_currentUserId);
	QVERIFY(imtdeskgql::HasTicketVisibility(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr));
}

void CTicketPermissionsTest::testHasTicketVisibility_SameGroup()
{
	m_userInfoPtr->groups = {"shared-group"};
	m_reporterGroupsPtr->groups = {"shared-group"};
	QVERIFY(imtdeskgql::HasTicketVisibility(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr));
}

void CTicketPermissionsTest::testHasTicketVisibility_NoAccess()
{
	// User is not admin, not reporter, not assignee, no shared groups
	m_userInfoPtr->groups = {"group-x"};
	m_reporterGroupsPtr->groups = {"group-y"};
	QVERIFY(!imtdeskgql::HasTicketVisibility(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr));
}


// --- CanEditTicket ---

void CTicketPermissionsTest::testCanEditTicket_Admin()
{
	m_userInfoPtr->isAdmin = true;
	QVERIFY(imtdeskgql::CanEditTicket(m_contextPtr, m_ticketPtr));
}

void CTicketPermissionsTest::testCanEditTicket_Reporter()
{
	m_ticketPtr->reporterId = m_currentUserId;
	QVERIFY(imtdeskgql::CanEditTicket(m_contextPtr, m_ticketPtr));
}

void CTicketPermissionsTest::testCanEditTicket_Assignee()
{
	m_ticketPtr->assigneeIds.append(m_currentUserId);
	QVERIFY(imtdeskgql::CanEditTicket(m_contextPtr, m_ticketPtr));
}

void CTicketPermissionsTest::testCanEditTicket_Viewer_ReturnsFalse()
{
	// Not admin, not reporter, not assignee
	QVERIFY(!imtdeskgql::CanEditTicket(m_contextPtr, m_ticketPtr));
}


// --- CanLockTicket ---

void CTicketPermissionsTest::testCanLockTicket_Admin()
{
	m_userInfoPtr->isAdmin = true;
	QVERIFY(imtdeskgql::CanLockTicket(m_contextPtr, m_ticketPtr));
}

void CTicketPermissionsTest::testCanLockTicket_Reporter()
{
	m_ticketPtr->reporterId = m_currentUserId;
	QVERIFY(imtdeskgql::CanLockTicket(m_contextPtr, m_ticketPtr));
}

void CTicketPermissionsTest::testCanLockTicket_Assignee_ReturnsFalse()
{
	m_ticketPtr->assigneeIds.append(m_currentUserId);
	QVERIFY(!imtdeskgql::CanLockTicket(m_contextPtr, m_ticketPtr));
}


// --- GetTicketAccessLevel ---

void CTicketPermissionsTest::testGetTicketAccessLevel_Admin_FullAccess()
{
	m_userInfoPtr->isAdmin = true;
	QCOMPARE(imtdeskgql::GetTicketAccessLevel(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr),
		imtdeskgql::FullAccess);
}

void CTicketPermissionsTest::testGetTicketAccessLevel_Reporter_FullAccess()
{
	m_ticketPtr->reporterId = m_currentUserId;
	QCOMPARE(imtdeskgql::GetTicketAccessLevel(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr),
		imtdeskgql::FullAccess);
}

void CTicketPermissionsTest::testGetTicketAccessLevel_Assignee_LimitedAccess()
{
	m_ticketPtr->assigneeIds.append(m_currentUserId);
	QCOMPARE(imtdeskgql::GetTicketAccessLevel(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr),
		imtdeskgql::LimitedAccess);
}

void CTicketPermissionsTest::testGetTicketAccessLevel_SameGroup_CommentOnly()
{
	m_userInfoPtr->groups = {"shared-group"};
	m_reporterGroupsPtr->groups = {"shared-group"};
	QCOMPARE(imtdeskgql::GetTicketAccessLevel(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr),
		imtdeskgql::CommentOnly);
}

void CTicketPermissionsTest::testGetTicketAccessLevel_Other_ViewOnly()
{
	m_userInfoPtr->groups = {"group-x"};
	m_reporterGroupsPtr->groups = {"group-y"};
	QCOMPARE(imtdeskgql::GetTicketAccessLevel(m_contextPtr, m_ticketPtr, m_reporterGroupsPtr),
		imtdeskgql::ViewOnly);
}

void CTicketPermissionsTest::testGetTicketAccessLevel_NullTicket_ViewOnly()
{
	QCOMPARE(imtdeskgql::GetTicketAccessLevel(m_contextPtr, nullptr, m_reporterGroupsPtr),
		imtdeskgql::ViewOnly);
}
