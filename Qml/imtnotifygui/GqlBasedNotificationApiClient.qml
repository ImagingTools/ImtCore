// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtnotifyNotificationsSdl 1.0

/**
 * GqlBasedNotificationApiClient
 *
 * GraphQL implementation of the NotificationApiClient contract (must mirror its signals and functions).
 * The only place of the notification center that imports the Notifications SDL module.
 * SDL objects are turned into the flat notification objects of the contract here.
 */
QtObject {
	id: root

	// Contract (see NotificationApiClient.qml)
	signal syncReceived(var items, var undelivered, int totalCount, int unreadCount, int undeliveredCount, string syncedAt)
	signal listReceived(string requestId, var items, int offset, int totalCount, int unreadCount)
	signal eventReceived(string kind, var items, var ids, int unreadCount, string createdUntil)
	signal unreadCountReceived(int unreadCount)
	signal typesReceived(var types, var objectTypes, string languageId)
	signal changed(var changedIds, int unreadCount)
	signal refreshRequired()
	signal requestFailed(string message)

	// Every request belongs to the session it was sent in: stop() starts a new one, so an answer that comes later, e.g.
	// after a switch of the tenant or a logout, is dropped
	property int __generation: 0

	function start(){
		root.__eventSubscription.registerSubscription()
	}

	function stop(){
		root.__generation++
		root.__eventSubscription.unRegisterSubscription()
	}

	function sync(pageSize, maxUndelivered){
		var request = root.__syncRequestComp.createObject(root, {"generation": root.__generation})
		var input = root.__syncInputComp.createObject(request)
		input.m_pageSize = pageSize
		input.m_maxUndelivered = maxUndelivered

		request.send(input)
	}

	// One request object per call: the bell and the page may ask at the same time
	function list(requestId, query){
		var request = root.__listRequestComp.createObject(root, {
			"generation": root.__generation,
			"requestId": requestId,
			"offset": query.offset ? query.offset : 0
		})
		var input = root.__listInputComp.createObject(request)
		input.m_offset = query.offset ? query.offset : 0
		input.m_pageSize = query.pageSize ? query.pageSize : 25

		request.send(input)
	}

	function requestUnreadCount(){
		var request = root.__countRequestComp.createObject(root, {"generation": root.__generation})
		request.send()
	}

	function requestTypes(){
		var request = root.__typesRequestComp.createObject(root, {"generation": root.__generation})
		request.send()
	}

	function markRead(ids){
		var request = root.__changeRequestComp.createObject(root, {
			"generation": root.__generation,
			"gqlCommandId": ImtnotifyNotificationsSdlCommandIds.s_markNotificationsRead
		})
		var input = root.__idsInputComp.createObject(request)
		input.m_ids = ids

		request.send(input)
	}

	function markAllRead(createdUntil){
		var request = root.__changeRequestComp.createObject(root, {
			"generation": root.__generation,
			"gqlCommandId": ImtnotifyNotificationsSdlCommandIds.s_markAllNotificationsRead
		})
		var input = root.__markAllReadInputComp.createObject(request)
		input.m_createdUntil = createdUntil

		request.send(input)
	}

	// Acknowledging is best effort: an unacknowledged notification only pops up once more
	function acknowledgeDelivered(ids){
		var request = root.__changeRequestComp.createObject(root, {
			"generation": root.__generation,
			"gqlCommandId": ImtnotifyNotificationsSdlCommandIds.s_acknowledgeNotificationsDelivered,
			"isReported": false
		})
		var input = root.__idsInputComp.createObject(request)
		input.m_ids = ids

		request.send(input)
	}

	function acknowledgeAllDelivered(createdUntil){
		var request = root.__changeRequestComp.createObject(root, {
			"generation": root.__generation,
			"gqlCommandId": ImtnotifyNotificationsSdlCommandIds.s_acknowledgeAllNotificationsDelivered,
			"isReported": false
		})
		var input = root.__acknowledgeAllInputComp.createObject(request)
		input.m_createdUntil = createdUntil

		request.send(input)
	}

	function remove(ids){
		var request = root.__changeRequestComp.createObject(root, {
			"generation": root.__generation,
			"gqlCommandId": ImtnotifyNotificationsSdlCommandIds.s_removeNotifications
		})
		var input = root.__idsInputComp.createObject(request)
		input.m_ids = ids

		request.send(input)
	}

	function __isCurrent(request){
		return request.generation === root.__generation
	}

	// Conversion of the SDL objects

	function __toItem(notification){
		var item = {
			"id": String(notification.m_id),
			"typeId": String(notification.m_typeId),
			"severity": String(notification.m_severity),
			"body": String(notification.m_body),
			"actors": root.__toLinks(notification.m_actors),
			"subjectId": "",
			"subjectTypeId": "",
			"subjectName": "",
			"arguments": root.__toArguments(notification.m_arguments),
			"groupCount": notification.m_groupCount,
			"createdAt": String(notification.m_createdAt),
			"isDelivered": notification.m_isDelivered,
			"isRead": notification.m_isRead
		}

		if (notification.m_subject){
			item.subjectId = String(notification.m_subject.m_id)
			item.subjectTypeId = String(notification.m_subject.m_typeId)
			item.subjectName = String(notification.m_subject.m_name)
		}

		return item
	}

	function __toLinks(model){
		var links = []
		if (model){
			for (var i = 0; i < model.count; i++){
				var link = model.get(i).item
				links.push({
					"id": link.m_id ? String(link.m_id) : "",
					"typeId": link.m_typeId ? String(link.m_typeId) : "",
					"name": link.m_name ? String(link.m_name) : ""
				})
			}
		}

		return links
	}

	function __toArguments(model){
		var argumentsList = []
		if (model){
			for (var i = 0; i < model.count; i++){
				var argument = model.get(i).item
				argumentsList.push({
					"id": String(argument.m_id),
					"kind": String(argument.m_kind),
					"value": argument.m_value ? String(argument.m_value) : "",
					"objectId": argument.m_objectId ? String(argument.m_objectId) : "",
					"objectTypeId": argument.m_objectTypeId ? String(argument.m_objectTypeId) : ""
				})
			}
		}

		return argumentsList
	}

	function __toItems(model){
		var items = []
		if (model){
			for (var i = 0; i < model.count; i++){
				items.push(root.__toItem(model.get(i).item))
			}
		}

		return items
	}

	function __toTypes(model){
		var types = []
		if (model){
			for (var i = 0; i < model.count; i++){
				var typeInfo = model.get(i).item
				types.push({
					"id": String(typeInfo.m_id),
					"name": typeInfo.m_name ? String(typeInfo.m_name) : "",
					"icon": typeInfo.m_icon ? String(typeInfo.m_icon) : "",
					"message": typeInfo.m_message ? String(typeInfo.m_message) : ""
				})
			}
		}

		return types
	}

	function __toIds(ids){
		var result = []
		if (ids){
			for (var i = 0; i < ids.length; i++){
				result.push(String(ids[i]))
			}
		}

		return result
	}

	// Requests: one object per call, destroyed with its answer

	property Component __syncInputComp: Component {
		NotificationsSyncInput {}
	}
	property Component __listInputComp: Component {
		NotificationsListInput {}
	}
	property Component __idsInputComp: Component {
		NotificationIdsInput {}
	}
	property Component __markAllReadInputComp: Component {
		MarkAllNotificationsReadInput {}
	}
	property Component __acknowledgeAllInputComp: Component {
		AcknowledgeAllNotificationsDeliveredInput {}
	}

	property Component __syncRequestComp: Component {
		GqlSdlRequestSender {
			id: syncRequest

			property int generation: 0

			gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_notificationsSync
			sdlObjectComp: Component {
				NotificationsSyncPayload {
					onFinished: {
						if (root.__isCurrent(syncRequest)){
							root.syncReceived(root.__toItems(m_items), root.__toItems(m_undelivered), m_totalCount, m_unreadCount, m_undeliveredCount, String(m_syncedAt))
						}

						syncRequest.destroy()
					}
				}
			}

			function onError(message, type){
				if (root.__isCurrent(syncRequest)){
					root.requestFailed(message)
				}

				syncRequest.destroy()
			}
		}
	}

	property Component __listRequestComp: Component {
		GqlSdlRequestSender {
			id: listRequest

			property int generation: 0
			property string requestId: ""
			property int offset: 0

			gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_notificationsList
			sdlObjectComp: Component {
				NotificationsListPayload {
					onFinished: {
						if (root.__isCurrent(listRequest)){
							root.listReceived(listRequest.requestId, root.__toItems(m_items), listRequest.offset, m_totalCount, m_unreadCount)
						}

						listRequest.destroy()
					}
				}
			}

			function onError(message, type){
				if (root.__isCurrent(listRequest)){
					root.requestFailed(message)
				}

				listRequest.destroy()
			}
		}
	}

	property Component __countRequestComp: Component {
		GqlSdlRequestSender {
			id: countRequest

			property int generation: 0

			gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_notificationsUnreadCount
			sdlObjectComp: Component {
				NotificationsUnreadCountPayload {
					onFinished: {
						if (root.__isCurrent(countRequest)){
							root.unreadCountReceived(m_unreadCount)
						}

						countRequest.destroy()
					}
				}
			}

			function onError(message, type){
				if (root.__isCurrent(countRequest)){
					root.requestFailed(message)
				}

				countRequest.destroy()
			}
		}
	}

	property Component __typesRequestComp: Component {
		GqlSdlRequestSender {
			id: typesRequest

			property int generation: 0

			gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_notificationTypes
			sdlObjectComp: Component {
				NotificationTypesPayload {
					onFinished: {
						if (root.__isCurrent(typesRequest)){
							root.typesReceived(root.__toTypes(m_types), root.__toTypes(m_objectTypes), m_languageId ? String(m_languageId) : "")
						}

						typesRequest.destroy()
					}
				}
			}

			function onError(message, type){
				if (root.__isCurrent(typesRequest)){
					root.requestFailed(message)
				}

				typesRequest.destroy()
			}
		}
	}

	// A change of the inbox made by this session; isReported: the answer is told by changed() and a failure by requestFailed()
	property Component __changeRequestComp: Component {
		GqlSdlRequestSender {
			id: changeRequest

			property int generation: 0
			property bool isReported: true

			requestType: 1
			sdlObjectComp: Component {
				NotificationsChangedPayload {
					onFinished: {
						if (changeRequest.isReported && root.__isCurrent(changeRequest)){
							root.changed(root.__toIds(m_changedIds), m_unreadCount)
						}

						changeRequest.destroy()
					}
				}
			}

			function onError(message, type){
				if (changeRequest.isReported && root.__isCurrent(changeRequest)){
					root.requestFailed(message)
				}

				changeRequest.destroy()
			}
		}
	}

	// Pushed changes

	// A new object per message: a field missing in the message must not keep the value of the previous one
	property Component __eventComp: Component {
		NotificationEvent {}
	}

	property SubscriptionClient __eventSubscription: SubscriptionClient {
		gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_onNotificationEvent
		autoSubscribe: false

		onMessageReceived: {
			if (!data){
				return
			}

			var event = root.__eventComp.createObject(root)
			event.fromJSON(data.toJson())

			var kind = String(event.m_kind)
			var items = root.__toItems(event.m_items)
			var ids = root.__toIds(event.m_ids)
			var unreadCount = event.m_unreadCount
			var createdUntil = event.m_createdUntil ? String(event.m_createdUntil) : ""

			event.destroy()

			root.eventReceived(kind, items, ids, unreadCount, createdUntil)
		}

		onRefreshRequired: {
			root.refreshRequired()
		}
	}
}
