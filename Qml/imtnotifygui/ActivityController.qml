// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtnotifygui 1.0

// Central controller for the "Events" (Activity) feature.
//
// Owns the single source of truth for the user's notifications: the unread
// count that drives the menu indicator, the list model shown on the Events
// page, and the current filter / sort / pagination state. It contains NO
// transport code: every server interaction goes through the injected abstract
// apiClient (NotificationApiClient). The application shell injects a concrete
// client and calls start() after login / stop() after logout.
QtObject {
	id: controller

	// Page id used both in the navigation config and for the menu badge event.
	readonly property string pageId: "Events"

	// Injected transport (a NotificationApiClient implementation).
	property var apiClient: null

	// Live list model for the Events page (a plain ListModel of item rows).
	property ListModel itemsModel: ListModel {}

	// Counters.
	property int unreadCount: 0
	property int totalCount: 0

	// Current server-side filter / sort state.
	property bool filterUnreadOnly: false
	property int filterSeverity: -1          // -1 = any; else INotification severity index
	property int sortOrder: 0                // 0 = newest first, 1 = oldest first
	readonly property int pageSize: 25

	// --- Public API ---------------------------------------------------------

	function start(){
		if (controller.apiClient){
			controller.apiClient.start()
		}
		controller.reload()
	}

	function stop(){
		if (controller.apiClient){
			controller.apiClient.stop()
		}
		controller.itemsModel.clear()
		controller.unreadCount = 0
		controller.totalCount = 0
		controller.__publishBadge()
	}

	// Re-apply the current filter from the first page.
	function reload(){
		if (!controller.apiClient){
			return
		}
		controller.apiClient.fetchList(controller.__buildFilter(0))
	}

	function loadMore(){
		if (!controller.apiClient){
			return
		}
		if (controller.itemsModel.count >= controller.totalCount){
			return
		}
		controller.apiClient.fetchList(controller.__buildFilter(controller.itemsModel.count))
	}

	function setUnreadOnly(unreadOnly){
		controller.filterUnreadOnly = unreadOnly
		controller.reload()
	}

	function setSeverity(severity){
		controller.filterSeverity = severity
		controller.reload()
	}

	function setSortOrder(order){
		controller.sortOrder = order
		controller.reload()
	}

	function markRead(notificationId){
		if (controller.apiClient && notificationId){
			controller.apiClient.markRead(notificationId)
		}
	}

	function markAllRead(){
		if (controller.apiClient){
			controller.apiClient.markAllRead()
		}
	}

	function remove(notificationId){
		if (controller.apiClient && notificationId){
			controller.apiClient.remove(notificationId)
		}
	}

	// Mark read and deep-link to the producing entity, if any. Navigation
	// mirrors the global Search: the notification's targetRoute is the path and
	// the source entity is passed as contextId / contextTypeId so the
	// destination page can open exactly the right document (see SearchPage.qml).
	function openNotification(notificationId, targetRoute, sourceType, sourceId, title){
		controller.markRead(notificationId)
		if (targetRoute && targetRoute !== ""){
			var params = {
				"contextId": sourceId ? sourceId : "",
				"contextTypeId": sourceType ? sourceType : "",
				"resultName": title ? title : ""
			}
			NavigationController.navigate(targetRoute, params)
		}
	}

	// --- Internal helpers ---------------------------------------------------

	function __buildFilter(offset){
		return {
			"unreadOnly": controller.filterUnreadOnly,
			"category": "",
			"severity": controller.filterSeverity,
			"sortOrder": controller.sortOrder,
			"offset": offset,
			"count": controller.pageSize
		}
	}

	function __publishBadge(){
		Events.sendEvent("UpdatePageBadge", {"id": controller.pageId, "count": controller.unreadCount})
	}

	function __appendItems(items){
		for (var i = 0; i < items.length; ++i){
			controller.itemsModel.append(items[i])
		}
	}

	// --- Reactions to the transport ----------------------------------------

	property Connections __apiConnections: Connections {
		target: controller.apiClient

		function onListReceived(items, unreadCount, totalCount, offset){
			if (offset === 0){
				controller.itemsModel.clear()
			}
			controller.__appendItems(items)
			controller.unreadCount = unreadCount
			controller.totalCount = totalCount
			controller.__publishBadge()
		}

		function onUnreadCountReceived(unreadCount){
			controller.unreadCount = unreadCount
			controller.__publishBadge()
		}

		function onNotificationChanged(){
			controller.reload()
		}

		function onNotificationReceived(notification){
			controller.__raisePopup(notification)
			controller.unreadCount = controller.unreadCount + 1
			controller.__publishBadge()
			controller.reload()
		}

		function onRequestFailed(message){
			if (message && message !== ""){
				PopupManager.addErrorMessage(message, true)
			}
		}
	}

	// --- Global popup for live notifications --------------------------------

	function __raisePopup(notification){
		if (!notification){
			return
		}
		PopupManager.addCustomMessage(notification.id, controller.notificationPopupComp, {
			autoClose: true,
			notificationId: notification.id,
			title: notification.title,
			preview: notification.preview,
			iconName: notification.iconName,
			severity: notification.severity,
			sourceType: notification.sourceType,
			sourceId: notification.sourceId,
			targetRoute: notification.targetRoute
		})
	}

	property Component notificationPopupComp: NotificationPopup {}
}
