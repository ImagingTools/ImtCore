// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
 * One notification of a list: severity mark, icon, text, time, unread dot and remove button.
 * A click opens the notification through NotificationCenter.
 */
Rectangle {
	id: notificationDelegate

	// A row of a notification list
	property var notificationItem: null
	property bool removable: true

	// What handles open(item), markRead(id) and remove(id): NotificationCenter, or the controller of a page with its own list
	property var actions: NotificationCenter

	signal opened()

	readonly property bool isRead: notificationDelegate.notificationItem ? notificationDelegate.notificationItem.isRead : true

	width: parent ? parent.width : Style.sizeHintS
	height: contentColumn.height + 2 * Style.marginM
	color: mouseArea.containsMouse ? Style.alternateBaseColor : Style.baseColor

	function severityColor(){
		if (!notificationDelegate.notificationItem){
			return "transparent"
		}

		var severity = notificationDelegate.notificationItem.severity
		if (severity === "Critical" || severity === "Error"){
			return Style.errorColor
		}

		if (severity === "Warning"){
			return Style.attentionColor
		}

		return "transparent"
	}

	function timeText(){
		if (!notificationDelegate.notificationItem || notificationDelegate.notificationItem.createdAt === ""){
			return ""
		}

		return Qt.formatDateTime(new Date(notificationDelegate.notificationItem.createdAt), "dd.MM.yyyy hh:mm")
	}

	function titleText(){
		return notificationDelegate.notificationItem ? NotificationTypeRegistry.getTitle(notificationDelegate.notificationItem) : ""
	}

	function detailsText(){
		return notificationDelegate.notificationItem ? NotificationTypeRegistry.getDetails(notificationDelegate.notificationItem) : ""
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor

		onClicked: {
			if (notificationDelegate.notificationItem){
				notificationDelegate.actions.open(notificationDelegate.notificationItem)
				notificationDelegate.opened()
			}
		}
	}

	Rectangle {
		id: severityMark

		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		width: Style.marginXS
		color: notificationDelegate.severityColor()
	}

	Image {
		id: icon

		anchors.left: severityMark.right
		anchors.leftMargin: Style.marginM
		anchors.top: parent.top
		anchors.topMargin: Style.marginM

		width: Style.iconSizeS
		height: width
		sourceSize.width: width
		sourceSize.height: height
		source: notificationDelegate.notificationItem
				? Style.getIconPath(NotificationTypeRegistry.typesRevision >= 0 ? NotificationTypeRegistry.getIcon(notificationDelegate.notificationItem) : "", Icon.State.On, Icon.Mode.Normal)
				: ""
	}

	Column {
		id: contentColumn

		anchors.left: icon.right
		anchors.leftMargin: Style.marginM
		anchors.right: unreadDot.left
		anchors.rightMargin: Style.marginM
		anchors.top: parent.top
		anchors.topMargin: Style.marginM

		spacing: Style.marginXS

		BaseText {
			textFormat: Text.PlainText
			width: parent.width

			text: NotificationTypeRegistry.typesRevision >= 0 ? notificationDelegate.titleText() : ""
			font.bold: !notificationDelegate.isRead
			elide: Text.ElideRight
			maximumLineCount: 2
			wrapMode: Text.WordWrap
		}

		BaseText {
			textFormat: Text.PlainText
			width: parent.width

			text: NotificationTypeRegistry.typesRevision >= 0 ? notificationDelegate.detailsText() : ""
			color: Style.subtitleColor
			elide: Text.ElideRight
		}

		BaseText {
			textFormat: Text.PlainText
			width: parent.width

			visible: text !== ""
			text: notificationDelegate.notificationItem ? notificationDelegate.notificationItem.body.split("\n").join(" ") : ""
			color: Style.subtitleColor
			elide: Text.ElideRight
			maximumLineCount: 2
			wrapMode: Text.WordWrap
		}

		BaseText {
			textFormat: Text.PlainText
			text: notificationDelegate.timeText()
			color: Style.inactiveTextColor
			font.pixelSize: Style.fontSizeS
		}
	}

	Rectangle {
		id: unreadDot

		anchors.right: markReadButton.left
		anchors.rightMargin: Style.marginS
		anchors.verticalCenter: icon.verticalCenter

		width: Style.marginS
		height: width
		radius: width / 2
		color: Style.highlightColor
		opacity: notificationDelegate.isRead ? 0 : 1
	}

	ToolButton {
		id: markReadButton

		anchors.right: removeButton.left
		anchors.verticalCenter: icon.verticalCenter

		width: Style.iconSizeS + Style.marginS
		height: width

		visible: !notificationDelegate.isRead
		opacity: mouseArea.containsMouse || markReadButton.hovered ? 1 : 0
		tooltipText: qsTr("Mark as read")
		iconSource: "qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)

		onClicked: {
			if (notificationDelegate.notificationItem){
				notificationDelegate.actions.markRead(notificationDelegate.notificationItem.id)
			}
		}
	}

	ToolButton {
		id: removeButton

		anchors.right: parent.right
		anchors.rightMargin: Style.marginS
		anchors.verticalCenter: icon.verticalCenter

		width: Style.iconSizeS + Style.marginS
		height: width

		visible: notificationDelegate.removable
		opacity: mouseArea.containsMouse || removeButton.hovered ? 1 : 0
		tooltipText: qsTr("Remove")
		iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

		onClicked: {
			if (notificationDelegate.notificationItem){
				notificationDelegate.actions.remove(notificationDelegate.notificationItem.id)
			}
		}
	}
}
