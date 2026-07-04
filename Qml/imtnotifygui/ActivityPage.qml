// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtnotifygui 1.0

// The "Events" page: a chronological, server-filtered feed of the current
// user's notifications (new ticket messages, tenant invitations, ...). Always
// available regardless of permissions. Analogous to the Activity page in
// Microsoft Teams.
Rectangle {
	id: page

	anchors.fill: parent
	color: Style.backgroundColor

	Component.onCompleted: ActivityController.reload()

	// --- Header ------------------------------------------------------------
	Item {
		id: header

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: Style.headerHeight

		BaseText {
			id: titleText

			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Events")
			color: Style.titleColor
			font.bold: true
			font.pixelSize: Style.fontSizeXL
		}

		Rectangle {
			id: unreadBadge

			anchors.left: titleText.right
			anchors.leftMargin: Style.marginM
			anchors.verticalCenter: parent.verticalCenter
			height: Style.iconSizeS
			width: Math.max(height, unreadLabel.width + Style.marginM)
			radius: height / 2
			color: Style.highlightColor
			visible: ActivityController.unreadCount > 0

			BaseText {
				id: unreadLabel

				anchors.centerIn: parent
				text: ActivityController.unreadCount
				color: Style.baseColor
				font.pixelSize: Style.fontSizeXS
				font.bold: true
			}
		}

		TextButton {
			id: markAllButton

			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Mark all as read")
			enabled: ActivityController.unreadCount > 0
			onClicked: ActivityController.markAllRead()
		}

		Rectangle {
			id: headerSeparator

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			height: Style.buttonBorderWidth
			color: Style.borderColor
		}
	}

	// --- Filter / sort bar -------------------------------------------------
	NotificationFilterBar {
		id: filterBar

		anchors.top: header.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Style.marginXL
		anchors.rightMargin: Style.marginXL
		anchors.topMargin: Style.marginS
	}

	// --- List --------------------------------------------------------------
	ListView {
		id: listView

		anchors.top: filterBar.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.topMargin: Style.marginS
		clip: true
		model: ActivityController.itemsModel
		boundsBehavior: Flickable.StopAtBounds
		delegate: NotificationDelegate {}

		// "Load more" footer for server-side pagination.
		footer: Item {
			width: listView.width
			height: ActivityController.itemsModel.count < ActivityController.totalCount ? Style.controlHeightL + 2 * Style.marginM : 0
			visible: ActivityController.itemsModel.count < ActivityController.totalCount

			TextButton {
				id: loadMoreButton

				anchors.centerIn: parent
				text: qsTr("Load more")
				onClicked: ActivityController.loadMore()
			}
		}
	}

	// --- Empty state -------------------------------------------------------
	Column {
		id: emptyState

		anchors.centerIn: parent
		spacing: Style.marginL
		visible: ActivityController.itemsModel.count === 0

		Image {
			id: emptyIcon

			anchors.horizontalCenter: parent.horizontalCenter
			width: Style.iconSizeXL
			height: width
			opacity: Style.opacityHigh
			source: "../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal)
			sourceSize.width: width
			sourceSize.height: height
		}

		BaseText {
			id: emptyText

			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("You have no events yet")
			color: Style.inactiveTextColor
		}
	}
}
