// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

// Abstract transport contract for the Events (Activity) feature.
//
// ActivityController and the pages depend ONLY on this contract, never on a
// concrete transport (GraphQL, REST, ...). A concrete client — e.g.
// GqlNotificationApiClient in the imtnotifyguigql module — implements these
// functions and emits these signals, and is injected into ActivityController by
// the application shell. This keeps imtnotifygui free of any transport code so
// the transport can be swapped without touching the UI (Dependency Inversion).
QtObject {
	id: client

	// Emitted when a page of the feed arrives.
	//   items       : array of plain notification objects
	//   unreadCount : unread notifications for the user
	//   totalCount  : total notifications matching the current filter
	//   offset      : offset this page was requested with (0 => replace list)
	signal listReceived(var items, int unreadCount, int totalCount, int offset)

	// Emitted when only the unread count was requested.
	signal unreadCountReceived(int unreadCount)

	// Emitted after a successful mutation (mark read / mark all / delete).
	signal notificationChanged()

	// Emitted when a new notification is pushed live to this user.
	signal notificationReceived(var notification)

	// Emitted when any request fails.
	signal requestFailed(string message)

	// --- Overridable operations (no-ops in the abstract contract) ---

	// Begin / end the live subscription (called on login / logout).
	function start(){}
	function stop(){}

	// Fetch a page. filter = { unreadOnly, category, severity, sortOrder,
	// offset, count }; severity/sortOrder are integers, -1/0 defaults.
	function fetchList(filter){}

	// Fetch only the unread count.
	function fetchUnreadCount(){}

	// Mutations.
	function markRead(notificationId){}
	function markAllRead(){}
	function remove(notificationId){}
}
