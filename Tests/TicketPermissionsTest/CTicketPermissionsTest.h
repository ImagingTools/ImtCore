// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtTest/QtTest>

#include <memory>


namespace imtdeskgql
{


/**
	Ticket access level enum (mirrors sdl::V1_0::imtdesk::TicketAccessLevel).
*/
enum TicketAccessLevel
{
	FullAccess = 0,
	LimitedAccess = 1,
	CommentOnly = 2,
	ViewOnly = 3
};


struct MockUserInfo
{
	QByteArray userId;
	bool isAdmin = false;
	QByteArrayList groups;
};

struct MockTicket
{
	QByteArray id;
	QString title;
	QByteArray reporterId;
	QByteArrayList assigneeIds;
};

struct MockContext
{
	MockUserInfo* userInfoPtr = nullptr;

	QByteArray GetUserId() const
	{
		return userInfoPtr ? userInfoPtr->userId : QByteArray();
	}
};

// Reporter's groups (simulates lookup from user collection)
struct MockReporterGroups
{
	QByteArrayList groups;
};


// --- Permission logic (mirrors TicketPermissions.h) ---

inline bool IsCurrentUserAdmin(const MockContext* contextPtr)
{
	if (contextPtr == nullptr || contextPtr->userInfoPtr == nullptr){
		return false;
	}
	return contextPtr->userInfoPtr->isAdmin;
}

inline bool IsCurrentUserRelatedToTicket(
	const MockContext* contextPtr,
	const MockTicket* ticketPtr,
	bool& isReporter,
	bool& isAssignee)
{
	isReporter = false;
	isAssignee = false;

	if (contextPtr == nullptr || ticketPtr == nullptr){
		return false;
	}

	QByteArray userId = contextPtr->GetUserId();
	if (userId.isEmpty()){
		return false;
	}

	if (ticketPtr->reporterId == userId){
		isReporter = true;
	}
	if (ticketPtr->assigneeIds.contains(userId)){
		isAssignee = true;
	}
	return isReporter || isAssignee;
}

inline bool IsCurrentUserInSameGroupAsReporter(
	const MockContext* contextPtr,
	const MockReporterGroups* reporterGroupsPtr)
{
	if (contextPtr == nullptr || contextPtr->userInfoPtr == nullptr || reporterGroupsPtr == nullptr){
		return false;
	}

	const QByteArrayList& userGroups = contextPtr->userInfoPtr->groups;
	if (userGroups.isEmpty() || reporterGroupsPtr->groups.isEmpty()){
		return false;
	}

	for (const QByteArray& groupId : reporterGroupsPtr->groups){
		if (userGroups.contains(groupId)){
			return true;
		}
	}
	return false;
}

inline bool HasTicketVisibility(
	const MockContext* contextPtr,
	const MockTicket* ticketPtr,
	const MockReporterGroups* reporterGroupsPtr)
{
	if (ticketPtr == nullptr){
		return false;
	}
	if (IsCurrentUserAdmin(contextPtr)){
		return true;
	}

	bool isReporter = false;
	bool isAssignee = false;
	if (IsCurrentUserRelatedToTicket(contextPtr, ticketPtr, isReporter, isAssignee)){
		return true;
	}

	return IsCurrentUserInSameGroupAsReporter(contextPtr, reporterGroupsPtr);
}

inline bool CanEditTicket(const MockContext* contextPtr, const MockTicket* ticketPtr)
{
	if (ticketPtr == nullptr){
		return false;
	}
	if (IsCurrentUserAdmin(contextPtr)){
		return true;
	}

	bool isReporter = false;
	bool isAssignee = false;
	IsCurrentUserRelatedToTicket(contextPtr, ticketPtr, isReporter, isAssignee);
	return isReporter || isAssignee;
}

inline bool CanLockTicket(const MockContext* contextPtr, const MockTicket* ticketPtr)
{
	if (ticketPtr == nullptr){
		return false;
	}
	if (IsCurrentUserAdmin(contextPtr)){
		return true;
	}

	bool isReporter = false;
	bool isAssignee = false;
	IsCurrentUserRelatedToTicket(contextPtr, ticketPtr, isReporter, isAssignee);
	return isReporter;
}

inline TicketAccessLevel GetTicketAccessLevel(
	const MockContext* contextPtr,
	const MockTicket* ticketPtr,
	const MockReporterGroups* reporterGroupsPtr)
{
	if (ticketPtr == nullptr){
		return ViewOnly;
	}
	if (IsCurrentUserAdmin(contextPtr)){
		return FullAccess;
	}

	bool isReporter = false;
	bool isAssignee = false;
	IsCurrentUserRelatedToTicket(contextPtr, ticketPtr, isReporter, isAssignee);

	if (isReporter){
		return FullAccess;
	}
	if (isAssignee){
		return LimitedAccess;
	}
	if (IsCurrentUserInSameGroupAsReporter(contextPtr, reporterGroupsPtr)){
		return CommentOnly;
	}
	return ViewOnly;
}


} // namespace imtdeskgql


class CTicketPermissionsTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// IsCurrentUserAdmin
	void testIsCurrentUserAdmin_True();
	void testIsCurrentUserAdmin_False();
	void testIsCurrentUserAdmin_NullContext();

	// IsCurrentUserRelatedToTicket
	void testIsCurrentUserRelatedToTicket_Reporter();
	void testIsCurrentUserRelatedToTicket_Assignee();
	void testIsCurrentUserRelatedToTicket_Both();
	void testIsCurrentUserRelatedToTicket_Unrelated();
	void testIsCurrentUserRelatedToTicket_NullContext();
	void testIsCurrentUserRelatedToTicket_NullTicket();

	// IsCurrentUserInSameGroupAsReporter
	void testIsCurrentUserInSameGroupAsReporter_SharedGroup();
	void testIsCurrentUserInSameGroupAsReporter_NoSharedGroup();
	void testIsCurrentUserInSameGroupAsReporter_EmptyGroups();

	// HasTicketVisibility
	void testHasTicketVisibility_Admin();
	void testHasTicketVisibility_Reporter();
	void testHasTicketVisibility_Assignee();
	void testHasTicketVisibility_SameGroup();
	void testHasTicketVisibility_NoAccess();

	// CanEditTicket
	void testCanEditTicket_Admin();
	void testCanEditTicket_Reporter();
	void testCanEditTicket_Assignee();
	void testCanEditTicket_Viewer_ReturnsFalse();

	// CanLockTicket
	void testCanLockTicket_Admin();
	void testCanLockTicket_Reporter();
	void testCanLockTicket_Assignee_ReturnsFalse();

	// GetTicketAccessLevel
	void testGetTicketAccessLevel_Admin_FullAccess();
	void testGetTicketAccessLevel_Reporter_FullAccess();
	void testGetTicketAccessLevel_Assignee_LimitedAccess();
	void testGetTicketAccessLevel_SameGroup_CommentOnly();
	void testGetTicketAccessLevel_Other_ViewOnly();
	void testGetTicketAccessLevel_NullTicket_ViewOnly();

private:
	imtdeskgql::MockContext* m_contextPtr = nullptr;
	imtdeskgql::MockUserInfo* m_userInfoPtr = nullptr;
	imtdeskgql::MockTicket* m_ticketPtr = nullptr;
	imtdeskgql::MockReporterGroups* m_reporterGroupsPtr = nullptr;
	QByteArray m_currentUserId;
	QByteArray m_reporterId;
};
