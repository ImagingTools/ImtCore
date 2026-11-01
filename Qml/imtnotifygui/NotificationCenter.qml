// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
pragma Singleton
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0

/**
 * NotificationCenter
 *
 * The one state of the notifications of the signed-in user in the selected tenant:
 * the unread count of the bell, the list of the panel and the page, the popups of new notifications.
 *
 * The product injects the transport once:
 *   NotificationCenter.apiClient = gqlBasedNotificationApiClient
 * From then on the center starts at login, reloads at a tenant switch and stops at logout.
 */
QtObject {
	id: root

	// A NotificationApiClient implementation
	property var apiClient: null

	// Route of the page with NotificationsPage, if the product has one
	property string pageRoute: ""

	readonly property int pageSize: 25

	// More undelivered notifications at login than this are shown as one summary popup
	readonly property int maxPopups: 3

	readonly property string __requestId: "NotificationCenter"
	readonly property string __summaryPopupId: "NotificationSummary"
	readonly property string __popupIdPrefix: "Notification_"

	// The notifications shown as popups, by popup id: a click opens the one of its popup
	property var __popupItems: ({})

	property bool active: false
	property bool loading: false
	property int unreadCount: 0
	property int totalCount: 0
	property ListModel itemsModel: ListModel {}

	// The object the user has open: its new notifications are read at once and do not pop up
	property string openedSubjectTypeId: ""
	property string openedSubjectId: ""

	signal notificationOpened(var item)

	// The inbox changed on the server: a pushed event, a change of this session or a reconnect
	signal inboxChanged()

	// Answer of requestTypes(): the catalog of the server, [{"id", "name", "icon"}] in the language of the client
	signal typesReceived(var types)

	onApiClientChanged: {
		if (root.apiClient && AuthorizationController.isLoggedIn()){
			root.start()
		}
	}

	// Views that do not import this module (the top panel) follow the state through Events
	onActiveChanged: {
		root.__publishState()
	}

	onUnreadCountChanged: {
		root.__publishBadge()
	}

	onPageRouteChanged: {
		root.__publishBadge()
	}

	Component.onCompleted: {
		Events.subscribeEvent("NotificationCenterStateRequested", root.__publishState)
		Events.subscribeEvent("OnLocalizationChanged", root.__onLocalizationChanged)
	}

	property Connections __popupConnections: Connections {
		target: PopupManager

		function onMessageClicked(id){
			if (id === root.__summaryPopupId){
				Events.sendEvent("OpenNotificationPanel")

				return
			}

			var item = root.__popupItems[id]
			if (item){
				delete root.__popupItems[id]
				root.open(item)
			}
		}
	}

	// The labels of the catalog are in the language of the client
	function __onLocalizationChanged(languageId){
		if (root.active){
			root.requestTypes()
		}
	}

	function __publishState(){
		Events.sendEvent("NotificationCenterActiveChanged", root.active)
	}

	// The unread count on the menu entry of the notifications page
	function __publishBadge(){
		if (root.pageRoute === ""){
			return
		}

		var badge = root.unreadCount > 99 ? "99+" : root.unreadCount > 0 ? String(root.unreadCount) : ""
		Events.sendEvent("UpdatePageVisualStatus", {"id": root.pageRoute, "badge": badge})
	}

	function start(){
		if (!root.apiClient){
			return
		}

		root.active = true
		root.apiClient.start()
		root.requestTypes()
		root.sync()
	}

	function stop(){
		if (root.apiClient){
			root.apiClient.stop()
		}

		root.__unreadCountTimer.stop()

		// The popups belong to the inbox of the session
		for (var popupId in root.__popupItems){
			PopupManager.closeMessage(popupId)
		}

		PopupManager.closeMessage(root.__summaryPopupId)
		root.__popupItems = {}

		root.active = false
		root.loading = false
		root.itemsModel.clear()
		root.unreadCount = 0
		root.totalCount = 0
	}

	// Load the first page again
	function sync(){
		if (!root.active){
			return
		}

		root.loading = true
		root.apiClient.sync(root.pageSize, root.maxPopups)
	}

	function loadMore(){
		if (!root.active || root.loading || root.itemsModel.count >= root.totalCount){
			return
		}

		root.loading = true
		root.apiClient.list(root.__requestId, {"offset": root.itemsModel.count, "pageSize": root.pageSize})
	}

	function requestTypes(){
		if (root.active){
			root.apiClient.requestTypes()
		}
	}

	function markRead(notificationId){
		root.markReadItems([notificationId])
	}

	function markReadItems(ids){
		// A popup or the page can show a notification that is not on the loaded page; the server still has to know
		var unreadIds = []
		for (var i = 0; i < ids.length; i++){
			var index = root.__findIndex(ids[i])
			if (index >= 0){
				if (root.itemsModel.get(index).isRead){
					continue
				}

				root.itemsModel.setProperty(index, "isRead", true)
			}

			unreadIds.push(ids[i])
		}

		if (unreadIds.length > 0){
			root.apiClient.markRead(unreadIds)
		}
	}

	function markAllRead(){
		if (!root.active || root.unreadCount === 0){
			return
		}

		// Up to the newest shown one, in server time: notifications the user has not seen yet stay unread
		var createdUntil = root.itemsModel.count > 0 ? root.itemsModel.get(0).createdAt : ""

		for (var i = 0; i < root.itemsModel.count; i++){
			root.itemsModel.setProperty(i, "isRead", true)
		}

		root.apiClient.markAllRead(createdUntil)
	}

	function remove(notificationId){
		root.removeItems([notificationId])
	}

	function removeItems(ids){
		if (ids.length === 0){
			return
		}

		for (var i = 0; i < ids.length; i++){
			var index = root.__findIndex(ids[i])
			if (index >= 0){
				root.itemsModel.remove(index)
				root.totalCount = Math.max(0, root.totalCount - 1)
			}
		}

		root.apiClient.remove(ids)
	}

	// Mark read and go to the object of the notification
	function open(item){
		root.markRead(item.id)

		var route = NotificationTypeRegistry.getRoute(item)
		if (route !== ""){
			NavigationController.navigate(route, {
				"contextId": item.subjectId,
				"contextTypeId": item.subjectTypeId
			})
		}

		root.notificationOpened(item)
	}

	// Called by a page when it shows an object, with empty values when it closes it
	function setOpenedSubject(subjectTypeId, subjectId){
		root.openedSubjectTypeId = subjectTypeId
		root.openedSubjectId = subjectId
	}

	function __findIndex(notificationId){
		for (var i = 0; i < root.itemsModel.count; i++){
			if (root.itemsModel.get(i).id === notificationId){
				return i
			}
		}

		return -1
	}

	function __isOpened(item){
		return root.openedSubjectId !== "" && item.subjectId === root.openedSubjectId && item.subjectTypeId === root.openedSubjectTypeId
	}

	function __showPopups(items){
		if (items.length > root.maxPopups){
			root.__showSummaryPopup(items.length)

			return
		}

		for (var i = 0; i < items.length; i++){
			root.__showPopup(items[i])
		}
	}

	function __showSummaryPopup(count){
		PopupManager.addInfoMessage(qsTr("%1 new notifications while you were away").arg(count), true, root.__summaryPopupId, {
			"clickable": true
		})
	}

	// Forget the notifications of the popups closed meanwhile
	function __prunePopupItems(){
		for (var popupId in root.__popupItems){
			if (!PopupManager.messageIsOpened(popupId)){
				delete root.__popupItems[popupId]
			}
		}
	}

	// A notification as a message of PopupManager: the name of its object, the label of its type, a click opens it
	function __showPopup(item){
		root.__prunePopupItems()

		var popupId = root.__popupIdPrefix + item.id
		var options = {
			"messageDetails": NotificationTypeRegistry.getDetails(item),
			"messageIcon": NotificationTypeRegistry.getIcon(item),
			"plainText": true,
			"clickable": true
		}

		root.__popupItems[popupId] = item

		var title = NotificationTypeRegistry.getTitle(item)
		if (item.severity === "Critical" || item.severity === "Error"){
			PopupManager.addErrorMessage(title, true, popupId, options)
		}
		else if (item.severity === "Warning"){
			PopupManager.addWarningMessage(title, true, popupId, options)
		}
		else{
			PopupManager.addInfoMessage(title, true, popupId, options)
		}
	}

	function __acknowledge(items){
		var ids = []
		for (var i = 0; i < items.length; i++){
			ids.push(items[i].id)
		}

		if (ids.length > 0){
			root.apiClient.acknowledgeDelivered(ids)
		}
	}

	function __applyEvent(kind, items, ids, unreadCount, createdUntil){
		if (kind === "Created" || kind === "Updated"){
			var popupItems = []
			for (var i = 0; i < items.length; i++){
				var item = items[i]
				var index = root.__findIndex(item.id)
				if (index >= 0){
					root.itemsModel.remove(index)
				}
				else{
					root.totalCount = root.totalCount + 1
				}

				root.itemsModel.insert(0, item)

				if (root.__isOpened(item)){
					root.markRead(item.id)
				}
				else{
					popupItems.push(item)
				}
			}

			root.__showPopups(popupItems)
			root.__acknowledge(items)
		}
		else if (kind === "AllRead"){
			// The times are ISO 8601 UTC texts of the same form: they compare as texts
			for (var a = 0; a < root.itemsModel.count; a++){
				if (root.itemsModel.get(a).createdAt <= createdUntil){
					root.itemsModel.setProperty(a, "isRead", true)
				}
			}
		}
		else if (kind === "Read"){
			for (var j = 0; j < ids.length; j++){
				var readIndex = root.__findIndex(ids[j])
				if (readIndex >= 0){
					root.itemsModel.setProperty(readIndex, "isRead", true)
				}
			}
		}
		else if (kind === "Removed"){
			for (var k = 0; k < ids.length; k++){
				var removedIndex = root.__findIndex(ids[k])
				if (removedIndex >= 0){
					root.itemsModel.remove(removedIndex)
					root.totalCount = Math.max(0, root.totalCount - 1)
				}
			}
		}

		if (unreadCount >= 0){
			root.unreadCount = unreadCount
		}
	}

	// The events that come together ask for the count once
	property Timer __unreadCountTimer: Timer {
		interval: 300

		onTriggered: {
			if (root.active){
				root.apiClient.requestUnreadCount()
			}
		}
	}

	property Connections __authorizationConnections: Connections {
		target: AuthorizationController

		function onLoggedIn(){
			root.start()
		}

		function onLoggedOut(){
			root.stop()
		}

		// The inbox and the subscription belong to the tenant of the session
		function onTenantSelected(tenantId){
			if (root.active){
				root.stop()
				root.start()
			}
		}
	}

	property Connections __apiConnections: Connections {
		target: root.apiClient

		// An answer arriving after the stop, e.g. after a logout, is not shown
		function onSyncReceived(items, undelivered, totalCount, unreadCount, undeliveredCount, syncedAt){
			if (!root.active){
				return
			}

			root.loading = false
			root.itemsModel.clear()
			for (var i = 0; i < items.length; i++){
				root.itemsModel.append(items[i])
			}

			root.totalCount = totalCount
			root.unreadCount = unreadCount

			// Only the newest undelivered ones came: more of them are summed up and acknowledged at once
			if (undeliveredCount > root.maxPopups){
				root.__showSummaryPopup(undeliveredCount)
				root.apiClient.acknowledgeAllDelivered(syncedAt)

				return
			}

			root.__showPopups(undelivered)
			root.__acknowledge(undelivered)
		}

		function onListReceived(requestId, items, offset, totalCount, unreadCount){
			if (requestId !== root.__requestId || !root.active){
				return
			}

			root.loading = false
			if (offset === 0){
				root.itemsModel.clear()
			}

			for (var i = 0; i < items.length; i++){
				root.itemsModel.append(items[i])
			}

			root.totalCount = totalCount
			root.unreadCount = unreadCount
		}

		function onEventReceived(kind, items, ids, unreadCount, createdUntil){
			if (!root.active){
				return
			}

			root.__applyEvent(kind, items, ids, unreadCount, createdUntil)

			// Personal notifications reach every inbox of the user, each session counts its own
			if (unreadCount < 0){
				root.__unreadCountTimer.restart()
			}

			root.inboxChanged()
		}

		function onUnreadCountReceived(unreadCount){
			if (root.active){
				root.unreadCount = unreadCount
			}
		}

		function onTypesReceived(types, objectTypes, languageId){
			if (root.active){
				NotificationTypeRegistry.setTypes(types, objectTypes, languageId)
				root.typesReceived(types)
			}
		}

		function onChanged(changedIds, unreadCount){
			if (!root.active){
				return
			}

			root.unreadCount = unreadCount
			root.inboxChanged()
		}

		function onRefreshRequired(){
			root.sync()
			root.inboxChanged()
		}

		function onRequestFailed(message){
			root.loading = false
			console.warn("NotificationCenter:", message)
		}
	}
}
