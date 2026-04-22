// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtgql/IGqlContext.h>


/**
\file TicketPermissions.h
\brief Free helper functions for ticket-related permission checks.

These helpers are shared between CTicketCollectionControllerComp (list
visibility filtering), CTicketCollectionDocumentManagerComp (document
get/update permission guards) and the ticket message notifier.

All helpers operate on an imtgql::IGqlContext pointer, which is the
common abstraction holding the current user info. The DocumentManager
obtains it via gqlRequest.GetRequestContext(); the controller and
notifier obtain it via imtgql::CGqlRequestContextManager::GetContext().
*/

namespace imtdeskgql
{


/**
Returns true if the current user is an administrator.
*/
inline bool IsCurrentUserAdmin(const imtgql::IGqlContext* contextPtr)
{
	if (contextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			return userInfoPtr->IsAdmin();
		}
	}
	return false;
}


/**
Determines whether the current user is the reporter or an assignee of the
given ticket. Returns true if the user is related to the ticket in either
role; sets \a isReporter and \a isAssignee accordingly.
*/
inline bool IsCurrentUserRelatedToTicket(
		const imtgql::IGqlContext* contextPtr,
		const imtdesk::ISupportTicket* ticketPtr,
		bool& isReporter,
		bool& isAssignee)
{
	isReporter = false;
	isAssignee = false;

	if (contextPtr == nullptr || ticketPtr == nullptr){
		return false;
	}

	const QByteArray userId = contextPtr->GetUserId();
	if (userId.isEmpty()){
		return false;
	}

	if (ticketPtr->GetReporterId() == userId){
		isReporter = true;
	}

	if (ticketPtr->GetAssigneeIds().contains(userId)){
		isAssignee = true;
	}

	return isReporter || isAssignee;
}


/**
Returns true if the current user shares at least one group with the
reporter identified by \a reporterId. Requires a valid user collection
(to look up the reporter's groups) and a user-group info provider.
*/
inline bool IsCurrentUserInSameGroupAsReporter(
		const imtgql::IGqlContext* contextPtr,
		const QByteArray& reporterId,
		const imtbase::IObjectCollection* userCollectionPtr,
		const imtauth::IUserGroupInfoProvider* userGroupInfoProviderPtr)
{
	if (reporterId.isEmpty() || userCollectionPtr == nullptr || userGroupInfoProviderPtr == nullptr){
		return false;
	}

	if (contextPtr == nullptr){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return false;
	}

	const QByteArrayList userGroups = userInfoPtr->GetGroups();
	if (userGroups.isEmpty()){
		return false;
	}

	QByteArrayList reporterGroups;
	imtbase::IObjectCollection::DataPtr reporterDataPtr;
	if (userCollectionPtr->GetObjectData(reporterId, reporterDataPtr)){
		const imtauth::IUserInfo* reporterInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(reporterDataPtr.GetPtr());
		if (reporterInfoPtr != nullptr){
			reporterGroups = reporterInfoPtr->GetGroups();
		}
	}

	if (reporterGroups.isEmpty()){
		return false;
	}

	for (const QByteArray& groupId : std::as_const(reporterGroups)){
		if (userGroups.contains(groupId)){
			return true;
		}
	}

	return false;
}


/**
Returns true if the current user can see the given ticket: admin, reporter,
assignee, or a user in the same group as the reporter.
*/
inline bool HasTicketVisibility(
		const imtgql::IGqlContext* contextPtr,
		const imtdesk::ISupportTicket* ticketPtr,
		const imtbase::IObjectCollection* userCollectionPtr,
		const imtauth::IUserGroupInfoProvider* userGroupInfoProviderPtr)
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

	return IsCurrentUserInSameGroupAsReporter(
			contextPtr, ticketPtr->GetReporterId(), userCollectionPtr, userGroupInfoProviderPtr);
}


/**
Returns true if the current user is allowed to edit the given ticket
(admin, reporter, or assignee).
*/
inline bool CanEditTicket(
		const imtgql::IGqlContext* contextPtr,
		const imtdesk::ISupportTicket* ticketPtr)
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


/**
Returns true if the current user is allowed to lock/unlock the given
ticket (admin or reporter).
*/
inline bool CanLockTicket(
		const imtgql::IGqlContext* contextPtr,
		const imtdesk::ISupportTicket* ticketPtr)
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


} // namespace imtdeskgql
