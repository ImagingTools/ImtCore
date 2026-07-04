// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtnotifyNotificationsSdl 1.0
import imtbaseImtCollectionSdl 1.0

// GQL/SDL implementation of the abstract NotificationApiClient contract.
//
// This is the ONLY place in the Events feature that imports the notification
// SDL module and owns GqlSdlRequestSender / SubscriptionClient instances.
// ActivityController depends solely on the abstract contract; the application
// shell injects this concrete client. Swapping the transport (e.g. to REST)
// means providing a different client here — no UI code changes.
QtObject {
	id: client

	// --- Contract signals (mirror NotificationApiClient) ---
	signal listReceived(var items, int unreadCount, int totalCount, int offset)
	signal unreadCountReceived(int unreadCount)
	signal notificationChanged()
	signal notificationReceived(var notification)
	signal requestFailed(string message)

	// Offset the in-flight list request was sent with (echoed back on arrival).
	property int __pendingOffset: 0

	// --- Contract operations ------------------------------------------------

	function start(){
		client.__subscription.registerSubscription()
	}

	function stop(){
		client.__subscription.unRegisterSubscription()
	}

	function fetchList(filter){
		client.__pendingOffset = filter.offset

		client.__viewParams.m_offset = filter.offset
		client.__viewParams.m_count = filter.count

		client.__listInput.m_unreadOnly = filter.unreadOnly
		client.__listInput.m_category = filter.category ? filter.category : ""
		client.__listInput.m_sortOrder = filter.sortOrder === 1 ? NotificationSortOrderEnum.s_oldestFirst : NotificationSortOrderEnum.s_newestFirst
		client.__listInput.m_severity = filter.severity
		client.__listInput.m_viewParams = client.__viewParams

		client.__listSender.send(client.__listInput)
	}

	function fetchUnreadCount(){
		client.__countSender.send(client.__countInput)
	}

	function markRead(notificationId){
		client.__markReadInput.m_id = notificationId
		client.__markReadInput.m_isRead = true
		client.__markReadSender.send(client.__markReadInput)
	}

	function markAllRead(){
		client.__markAllSender.send(client.__markAllInput)
	}

	function remove(notificationId){
		client.__deleteInput.m_id = notificationId
		client.__deleteSender.send(client.__deleteInput)
	}

	// --- List query ---------------------------------------------------------

	property CollectionViewParams __viewParams: CollectionViewParams {}
	property NotificationsListInput __listInput: NotificationsListInput {}
	property GqlSdlRequestSender __listSender: GqlSdlRequestSender {
		gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_notificationsList

		function onError(message, type){
			client.requestFailed(message)
		}

		sdlObjectComp: Component {
			NotificationsListPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== ""){
						client.requestFailed(m_errorMessage)
						return
					}
					client.__emitList(m_items, m_unreadCount, m_totalCount)
				}
			}
		}
	}

	function __emitList(items, unreadCount, totalCount){
		var result = []
		if (items){
			for (var i = 0; i < items.count; ++i){
				var item = items.get(i).item
				if (!item){
					continue
				}
				result.push({
					"id": item.m_id ? item.m_id : "",
					"category": item.m_category ? item.m_category : "",
					"title": item.m_title ? item.m_title : "",
					"preview": item.m_preview ? item.m_preview : "",
					"iconName": item.m_iconName ? item.m_iconName : "",
					"severity": item.m_severity ? item.m_severity : "Info",
					"isRead": item.m_isRead ? true : false,
					"sourceType": item.m_sourceType ? item.m_sourceType : "",
					"sourceId": item.m_sourceId ? item.m_sourceId : "",
					"targetRoute": item.m_targetRoute ? item.m_targetRoute : "",
					"createdAt": item.m_createdAt ? item.m_createdAt : ""
				})
			}
		}
		client.listReceived(result, unreadCount ? unreadCount : 0, totalCount ? totalCount : 0, client.__pendingOffset)
	}

	// --- Unread count query -------------------------------------------------

	property UnreadNotificationsCountInput __countInput: UnreadNotificationsCountInput {}
	property GqlSdlRequestSender __countSender: GqlSdlRequestSender {
		gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_unreadNotificationsCount

		function onError(message, type){
			client.requestFailed(message)
		}

		sdlObjectComp: Component {
			UnreadNotificationsCountPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== ""){
						client.requestFailed(m_errorMessage)
						return
					}
					client.unreadCountReceived(m_unreadCount ? m_unreadCount : 0)
				}
			}
		}
	}

	// --- Mutations ----------------------------------------------------------

	property MarkNotificationReadInput __markReadInput: MarkNotificationReadInput {}
	property GqlSdlRequestSender __markReadSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_markNotificationRead

		function onError(message, type){
			client.requestFailed(message)
		}

		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== ""){
						client.requestFailed(m_errorMessage)
						return
					}
					client.notificationChanged()
				}
			}
		}
	}

	property MarkAllNotificationsReadInput __markAllInput: MarkAllNotificationsReadInput {}
	property GqlSdlRequestSender __markAllSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_markAllNotificationsRead

		function onError(message, type){
			client.requestFailed(message)
		}

		sdlObjectComp: Component {
			MarkAllNotificationsReadPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== ""){
						client.requestFailed(m_errorMessage)
						return
					}
					client.notificationChanged()
				}
			}
		}
	}

	property DeleteNotificationInput __deleteInput: DeleteNotificationInput {}
	property GqlSdlRequestSender __deleteSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtnotifyNotificationsSdlCommandIds.s_deleteNotification

		function onError(message, type){
			client.requestFailed(message)
		}

		sdlObjectComp: Component {
			RemovedNotificationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== ""){
						client.requestFailed(m_errorMessage)
						return
					}
					client.notificationChanged()
				}
			}
		}
	}

	// --- Live subscription --------------------------------------------------

	property SubscriptionClient __subscription: SubscriptionClient {
		gqlCommandId: "OnNotificationReceived"
		autoSubscribe: false

		function getHeaders(){
			return {}
		}

		onMessageReceived: {
			if (!data){
				return
			}
			var notification = {
				"id": data.containsKey("id") ? data.getData("id") : "",
				"title": data.containsKey("title") ? data.getData("title") : "",
				"preview": data.containsKey("preview") ? data.getData("preview") : "",
				"iconName": data.containsKey("iconName") ? data.getData("iconName") : "",
				"severity": data.containsKey("severity") ? data.getData("severity") : "Info",
				"sourceType": data.containsKey("sourceType") ? data.getData("sourceType") : "",
				"sourceId": data.containsKey("sourceId") ? data.getData("sourceId") : "",
				"targetRoute": data.containsKey("targetRoute") ? data.getData("targetRoute") : ""
			}
			client.notificationReceived(notification)
		}
	}
}
