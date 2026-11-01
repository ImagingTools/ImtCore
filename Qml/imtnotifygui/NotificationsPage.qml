// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0

/**
 * Page with the whole collection of the notifications of the user in the current tenant and product.
 * The list is the server collection "Notifications": search, filters, sorting and paging are done by the server.
 * The commands come from the server (CommandsController "Notifications"); Remove is the standard one of the collection.
 * Laid out like the other collection pages: the standard panel with the commands and the filters, no own header.
 */
RemoteCollectionView {
	id: notificationsPage
	objectName: "NotificationsCollectionView"

	collectionId: "Notifications"
	gqlGetListCommandId: "UserNotificationsList"
	documentCollectionFilter: null
	showRemoteChangesAlert: false
	additionalFieldIds: ["id", "typeId", "severity", "body", "actors", "subject", "arguments",
		"groupCount", "createdAt", "isDelivered", "isRead"]

	// The inbox changed while the page was hidden: it is read again when shown
	property bool outdated: false

	onVisibleChanged: {
		if (notificationsPage.visible && notificationsPage.outdated){
			notificationsPage.outdated = false
			notificationsPage.doUpdateGui()
		}
	}

	Connections {
		target: NotificationCenter

		function onInboxChanged(){
			if (notificationsPage.visible){
				notificationsPage.doUpdateGui()
			}
			else{
				notificationsPage.outdated = true
			}
		}
	}

	// A row of the collection as the flat notification object of NotificationTypeRegistry and NotificationCenter
	function getItem(index){
		var elements = notificationsPage.table.elements

		return {
			"id": notificationsPage.getText(elements, "id", index),
			"typeId": notificationsPage.getText(elements, "typeId", index),
			"severity": notificationsPage.getText(elements, "severity", index),
			"body": notificationsPage.getText(elements, "body", index),
			"actors": notificationsPage.getList(elements, "actors", ["id", "typeId", "name"], index),
			"subjectId": notificationsPage.getLinkText(elements, "subject", "id", index),
			"subjectTypeId": notificationsPage.getLinkText(elements, "subject", "typeId", index),
			"subjectName": notificationsPage.getLinkText(elements, "subject", "name", index),
			"arguments": notificationsPage.getList(elements, "arguments", ["id", "kind", "value", "objectId", "objectTypeId"], index),
			"groupCount": Number(notificationsPage.getText(elements, "groupCount", index)),
			"createdAt": notificationsPage.getText(elements, "createdAt", index),
			"isDelivered": notificationsPage.getText(elements, "isDelivered", index) === "true",
			"isRead": notificationsPage.getText(elements, "isRead", index) === "true"
		}
	}

	function getText(elements, key, index){
		if (!elements || !elements.containsKey(key, index)){
			return ""
		}

		var value = elements.getData(key, index)

		return value === undefined || value === null ? "" : String(value)
	}

	// A list of a row as objects with the given keys
	function getList(elements, listKey, keys, index){
		var result = []
		if (!elements || !elements.containsKey(listKey, index)){
			return result
		}

		var list = elements.getData(listKey, index)
		if (!list){
			return result
		}

		for (var i = 0; i < list.count; i++){
			var entry = {}
			for (var j = 0; j < keys.length; j++){
				var value = list.containsKey(keys[j], i) ? list.getData(keys[j], i) : ""
				entry[keys[j]] = value === undefined || value === null ? "" : String(value)
			}

			result.push(entry)
		}

		return result
	}

	function getLinkText(elements, linkKey, key, index){
		if (!elements || !elements.containsKey(linkKey, index)){
			return ""
		}

		var link = elements.getData(linkKey, index)
		if (!link || !link.containsKey(key)){
			return ""
		}

		return String(link.getData(key))
	}

	function severityTitle(severity){
		if (severity === "Critical"){
			return qsTr("Critical")
		}

		if (severity === "Warning"){
			return qsTr("Warning")
		}

		if (severity === "Error"){
			return qsTr("Error")
		}

		return qsTr("Information")
	}

	Component.onCompleted: {
		notificationsPage.table.setSortingInfo("createdAt", "DESC")
		notificationsPage.registerFieldFilterDelegate("readFilter", readFilterComp)
		notificationsPage.registerFieldFilterDelegate("severityFilter", severityFilterComp)
		notificationsPage.registerFieldFilterDelegate("typeFilter", typeFilterComp)
	}

	onHeadersChanged: {
		notificationsPage.table.setColumnContentById("typeId", titleCellComp)
		notificationsPage.table.setColumnContentById("severity", severityCellComp)
		notificationsPage.table.setColumnContentById("createdAt", createdAtCellComp)
	}

	onCommandActivated: {
		if (!notificationsPage.commandsController || !notificationsPage.commandsController.commandIsEnabled(commandId)){
			return
		}

		if (commandId === "MarkAsRead"){
			NotificationCenter.markReadItems(notificationsPage.getSelectedIds())
		}
		else if (commandId === "MarkAllAsRead"){
			NotificationCenter.markAllRead()
		}
	}

	commandsDelegateComp: Component {
		CollectionViewCommandsDelegateBase {
			id: commandsDelegate

			collectionView: notificationsPage
			removeDialogTitle: qsTr("Removing notifications")
			removeMessage: qsTr("Remove the selected notifications?")

			function updateStateCustomCommands(selection, commandsController, elementsModel){
				var hasUnread = false
				for (var i = 0; i < selection.length; i++){
					if (notificationsPage.getText(elementsModel, "isRead", selection[i]) !== "true"){
						hasUnread = true
					}
				}

				commandsController.setCommandIsEnabled("MarkAsRead", hasUnread)
				commandsController.setCommandIsEnabled("MarkAllAsRead", NotificationCenter.unreadCount > 0)
			}

			Connections {
				target: NotificationCenter

				function onUnreadCountChanged(){
					commandsDelegate.updateItemSelection(notificationsPage.table.getSelectedIndexes())
				}
			}
		}
	}

	// Double click or Enter: mark read and go to the object of the notification
	function onEdit(id, index){
		NotificationCenter.open(notificationsPage.getItem(index))
	}
	Component {
		id: titleCellComp

		TableCellDelegateBase {
			id: titleCell

			onReused: {
				titleCell.updateTexts()
			}

			function updateTexts(){
				if (titleCell.rowIndex < 0){
					return
				}

				var item = notificationsPage.getItem(titleCell.rowIndex)
				titleIcon.source = "qrc:/" + Style.getIconPath(NotificationTypeRegistry.getIcon(item), Icon.State.On, Icon.Mode.Normal)
				titleLabel.text = NotificationTypeRegistry.getTitle(item)
				detailsLabel.text = NotificationTypeRegistry.getDetails(item)
				titleLabel.font.bold = !item.isRead
				unreadMark.visible = !item.isRead
			}

			// The labels come from the catalog, in the language of the client
			Connections {
				target: NotificationCenter

				function onTypesReceived(types){
					titleCell.updateTexts()
				}
			}

			Rectangle {
				id: unreadMark

				anchors.left: parent.left
				anchors.leftMargin: Style.marginS
				anchors.verticalCenter: parent.verticalCenter

				width: Style.marginS
				height: width
				radius: width / 2

				color: Style.imaginToolsAccentColor
			}

			Image {
				id: titleIcon

				anchors.left: unreadMark.right
				anchors.leftMargin: Style.marginS
				anchors.verticalCenter: parent.verticalCenter

				width: Style.iconSizeS
				height: width

				sourceSize.width: width
				sourceSize.height: height
			}

			BaseText {
				id: titleLabel
				textFormat: Text.PlainText

				anchors.left: titleIcon.right
				anchors.leftMargin: Style.marginS
				anchors.verticalCenter: parent.verticalCenter

				width: Math.min(implicitWidth, parent.width - x - Style.marginS)
				elide: Text.ElideRight
			}

			BaseText {
				id: detailsLabel
				textFormat: Text.PlainText

				anchors.left: titleLabel.right
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.rightMargin: Style.marginS
				anchors.verticalCenter: parent.verticalCenter

				color: Style.subtitleColor
				elide: Text.ElideRight
			}
		}
	}

	Component {
		id: severityCellComp

		TableCellDelegateBase {
			id: severityCell

			onReused: {
				severityLabel.text = notificationsPage.severityTitle(String(severityCell.getValue()))
			}

			BaseText {
				id: severityLabel
				textFormat: Text.PlainText

				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter

				elide: Text.ElideRight
			}
		}
	}

	Component {
		id: createdAtCellComp

		TableCellDateDelegate {}
	}

	Component {
		id: readFilterComp

		FieldFilterDelegate {
			name: qsTr("State")
			defaultFieldFilter.m_fieldId: "isRead"
			defaultFieldFilter.m_filterValueType: "Bool"

			Component.onCompleted: {
				createAndAddOption("false", qsTr("Unread"), "", true)
				createAndAddOption("true", qsTr("Read"), "", true)
			}
		}
	}

	Component {
		id: severityFilterComp

		FieldFilterDelegate {
			name: qsTr("Severity")
			defaultFieldFilter.m_fieldId: "severity"
			defaultFieldFilter.m_filterValueType: "Integer"

			Component.onCompleted: {
				// The values of istd::IInformationProvider::InformationCategory
				createAndAddOption("1", qsTr("Information"), "", true)
				createAndAddOption("2", qsTr("Warning"), "", true)
				createAndAddOption("3", qsTr("Error"), "", true)
				createAndAddOption("4", qsTr("Critical"), "", true)
			}
		}
	}

	Component {
		id: typeFilterComp

		FieldFilterDelegate {
			id: typeFilter

			name: qsTr("Type")
			defaultFieldFilter.m_fieldId: "typeId"

			// The labels shown for the type ids, from the catalog of the server
			property var typeNames: ({})

			Component.onCompleted: {
				NotificationCenter.requestTypes()
			}

			Connections {
				target: NotificationCenter

				function onTypesReceived(types){
					var typeNames = typeFilter.typeNames
					for (var i = 0; i < types.length; i++){
						var typeName = NotificationTypeRegistry.getTypeName(types[i].id)
						if (typeNames[types[i].id] === typeName){
							continue
						}

						// Another language: the option gets the new label
						if (typeNames[types[i].id] !== undefined){
							typeFilter.removeOption(types[i].id)
						}

						typeNames[types[i].id] = typeName
						typeFilter.createAndAddOption(types[i].id, typeName, "", true)
					}

					typeFilter.typeNames = typeNames
				}
			}
		}
	}
}
