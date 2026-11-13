// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
 * Bell with the unread count; opens NotificationPanel below itself.
 * Visible only while the notification center runs.
 */
Item {
	id: notificationBell

	width: Style.buttonWidthM
	height: width

	visible: NotificationCenter.active

	Component.onCompleted: {
		Events.subscribeEvent("OpenNotificationPanel", notificationBell.openPanel)
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OpenNotificationPanel", notificationBell.openPanel)
	}

	function openPanel(){
		if (!notificationBell.visible){
			return
		}

		var point = notificationBell.mapToItem(null, notificationBell.width, notificationBell.height)
		ModalDialogManager.openDialog(panelComp, {
			"x": Math.max(Style.marginS, point.x - Style.sizeHintS),
			"y": point.y + Style.marginXS
		})
	}

	ToolButton {
		id: bellButton
		objectName: "NotificationBell"

		anchors.fill: parent

		tooltipText: NotificationCenter.unreadCount > 0 ? qsTr("Notifications: %1 unread").arg(NotificationCenter.unreadCount) : qsTr("Notifications")
		iconSource: "qrc:/" + Style.getIconPath("Icons/Notification", Icon.State.On, Icon.Mode.Normal)

		onClicked: {
			notificationBell.openPanel()
		}
	}

	Rectangle {
		id: badge

		anchors.top: parent.top
		anchors.right: parent.right

		height: badgeText.height + Style.marginXS
		width: Math.max(height, badgeText.width + Style.marginS)
		radius: height / 2
		z: bellButton.z + 1

		visible: NotificationCenter.unreadCount > 0
		color: Style.imaginToolsAccentColor

		BaseText {
			id: badgeText

			anchors.centerIn: parent

			text: NotificationCenter.unreadCount > 99 ? "99+" : String(NotificationCenter.unreadCount)
			color: Style.highlightedTextColor
			font.pixelSize: Style.fontSizeXS
			font.bold: true
		}
	}

	Component {
		id: panelComp

		NotificationPanel {}
	}
}
