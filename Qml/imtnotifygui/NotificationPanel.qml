// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

/**
 * Drop-down panel of the bell: the latest notifications, "Mark all as read" and the way to the page.
 * Opened with ModalDialogManager.openDialog at the position of the bell.
 */
PopupView {
	id: notificationPanel

	readonly property int maxListHeight: 6 * (Style.controlHeightM + 2 * Style.marginL)

	width: Style.sizeHintS
	height: background.height
	forceFocus: true
	escapeEnabled: true

	function close(){
		if (notificationPanel.root){
			notificationPanel.root.closeDialog()
		}
	}

	Shortcut {
		sequence: "Escape"
		enabled: notificationPanel.escapeEnabled
		onActivated: {
			notificationPanel.close()
		}
	}

	Rectangle {
		id: background

		width: parent.width
		height: panelColumn.height

		color: Style.baseColor
		border.color: Style.borderColor
		radius: Style.radiusM
		clip: true

		Column {
			id: panelColumn

			width: parent.width

			Item {
				id: header

				width: parent.width
				height: Style.controlHeightM + 2 * Style.marginS

				BaseText {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter

					text: qsTr("Notifications")
					font.bold: true
				}

				Button {
					anchors.right: parent.right
					anchors.rightMargin: Style.marginS
					anchors.verticalCenter: parent.verticalCenter

					visible: NotificationCenter.unreadCount > 0
					text: qsTr("Mark all as read")

					onClicked: {
						NotificationCenter.markAllRead()
					}
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			Item {
				width: parent.width
				height: listView.count > 0 ? Math.min(listView.contentHeight, notificationPanel.maxListHeight) : emptyText.height + 2 * Style.marginL

				ListView {
					id: listView

					anchors.fill: parent

					clip: true
					boundsBehavior: Flickable.StopAtBounds
					model: NotificationCenter.itemsModel

					delegate: NotificationDelegate {
						width: listView.width
						notificationItem: model

						onOpened: {
							notificationPanel.close()
						}
					}

					onAtYEndChanged: {
						if (listView.atYEnd && listView.count > 0){
							NotificationCenter.loadMore()
						}
					}
				}

				CustomScrollbar {
					anchors.right: parent.right
					anchors.top: listView.top
					anchors.bottom: listView.bottom

					secondSize: Style.marginS
					targetItem: listView
				}

				BaseText {
					id: emptyText

					anchors.centerIn: parent

					visible: listView.count === 0
					text: qsTr("No notifications")
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
				visible: allButton.visible
			}

			Item {
				width: parent.width
				height: allButton.visible ? allButton.height + 2 * Style.marginS : 0

				Button {
					id: allButton

					anchors.centerIn: parent

					visible: NotificationCenter.pageRoute !== ""
					text: qsTr("All notifications")

					onClicked: {
						notificationPanel.close()
						NavigationController.navigate(NotificationCenter.pageRoute, {})
					}
				}
			}
		}
	}
}
