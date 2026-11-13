// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * NotificationApiClient
 *
 * Transport contract of the notification center. NotificationCenter depends only on it;
 * GqlBasedNotificationApiClient implements it over GraphQL and the OnNotificationEvent subscription.
 *
 * A notification is a flat object:
 *   id, typeId, severity ("Info" | "Warning" | "Error" | "Critical", the categories of ilog), body,
 *   actors ([{"id", "typeId", "name"}], the newest first), subjectId, subjectTypeId, subjectName,
 *   arguments ([{"id", "kind", "value", "objectId", "objectTypeId"}], the values of the message of the type),
 *   groupCount, createdAt, isDelivered, isRead
 *
 * The inbox is the one of the signed-in user in the selected tenant; the server takes it from the session.
 * A notification has no text in a language: the client shows the name of the subject and the label of the type from
 * the catalog (requestTypes); body is a text a user wrote.
 *
 * A list query is an object: offset, pageSize. The page of the notifications reads the collection "Notifications" instead.
 */
QtObject {
	id: root

	// Answer of sync(): the first page, the newest notifications no session has received yet (at most maxUndelivered),
	// the counters and syncedAt, the server time the answer was read at (for acknowledgeAllDelivered)
	signal syncReceived(var items, var undelivered, int totalCount, int unreadCount, int undeliveredCount, string syncedAt)

	// Answer of list(); requestId tells the callers apart
	signal listReceived(string requestId, var items, int offset, int totalCount, int unreadCount)

	// Pushed change of the inbox. kind: "Created" | "Updated" | "Read" | "AllRead" | "Removed".
	// items for Created and Updated, ids for Read and Removed, createdUntil for AllRead: the notifications created up to
	// it are read. A negative unreadCount asks for requestUnreadCount().
	signal eventReceived(string kind, var items, var ids, int unreadCount, string createdUntil)

	// Answer of requestUnreadCount()
	signal unreadCountReceived(int unreadCount)

	// Answer of requestTypes(): the catalog of the server in the language of the client, the notification types
	// [{"id", "name", "icon", "message"}], the types of their objects [{"id", "name"}] and that language
	signal typesReceived(var types, var objectTypes, string languageId)

	// Answer of a change made by this session
	signal changed(var changedIds, int unreadCount)

	// Pushed changes may have been missed, e.g. after a reconnect
	signal refreshRequired()

	signal requestFailed(string message)

	// Start and stop receiving the pushed changes. The answers to the requests sent before stop() are not told.
	function start(){}
	function stop(){}

	function sync(pageSize, maxUndelivered){}
	function list(requestId, query){}
	function requestUnreadCount(){}
	function requestTypes(){}
	function markRead(ids){}
	function markAllRead(createdUntil){}
	function acknowledgeDelivered(ids){}
	function acknowledgeAllDelivered(createdUntil){}
	function remove(ids){}
}
