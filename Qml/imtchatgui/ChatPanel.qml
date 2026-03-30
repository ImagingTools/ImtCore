// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: chatPanelContainer
	objectName: "ChatPanel"
	color: "transparent"

	property var conversationsModel: null
	property string currentConversationId: ""

	signal conversationSelected(string conversationId)
	signal newConversationRequested()

	Column {
		anchors.fill: parent
		spacing: 0

		// Header row
		Rectangle {
			width: parent.width
			height: Style.sizeS
			color: Style.panelHeaderColor

			Row {
				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
					leftMargin: Style.paddingM
					rightMargin: Style.paddingS
				}

				Text {
					text: qsTr("Chats")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textPrimaryColor
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width - newConvButton.width
				}

				Rectangle {
					id: newConvButton
					width: Style.iconSizeS
					height: Style.iconSizeS
					color: "transparent"
					anchors.verticalCenter: parent.verticalCenter

					Text {
						anchors.centerIn: parent
						text: "+"
						font.pixelSize: Style.fontSizeL
						color: Style.accentColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: chatPanelContainer.newConversationRequested()
					}
				}
			}
		}

		// Search bar
		ChatSearchBar {
			id: searchBar
			width: parent.width
			height: Style.inputHeightS
			placeholderText: qsTr("Search conversations...")
			onSearchTextChanged: function(text) {
				if (conversationsModel) {
					conversationsModel.filterText = text;
				}
			}
		}

		// Conversation list
		ListView {
			id: conversationListView
			width: parent.width
			height: parent.height - Style.sizeS - searchBar.height
			clip: true
			model: chatPanelContainer.conversationsModel

			delegate: Rectangle {
				width: conversationListView.width
				height: Style.listItemHeight
				color: model.id === chatPanelContainer.currentConversationId
					? Style.selectedItemColor
					: "transparent"

				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: 1
					color: Style.separatorColor
					opacity: 0.5
				}

				Row {
					anchors {
						left: parent.left
						right: parent.right
						verticalCenter: parent.verticalCenter
						leftMargin: Style.paddingM
						rightMargin: Style.paddingS
					}
					spacing: Style.paddingS

					// Avatar placeholder
					Rectangle {
						width: Style.avatarSizeS
						height: Style.avatarSizeS
						radius: Style.avatarSizeS / 2
						color: Style.accentColor
						anchors.verticalCenter: parent.verticalCenter

						Text {
							anchors.centerIn: parent
							text: model.name ? model.name.charAt(0).toUpperCase() : "?"
							font.pixelSize: Style.fontSizeS
							color: "white"
							font.bold: true
						}
					}

					Column {
						anchors.verticalCenter: parent.verticalCenter
						width: parent.width - Style.avatarSizeS - Style.paddingS

						Text {
							text: model.name || ""
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.textPrimaryColor
							elide: Text.ElideRight
							width: parent.width
						}

						Text {
							text: model.lastMessage || ""
							font.pixelSize: Style.fontSizeXS
							color: Style.textSecondaryColor
							elide: Text.ElideRight
							width: parent.width
						}
					}
				}

				// Unread badge
				Rectangle {
					visible: model.unreadCount > 0
					anchors {
						right: parent.right
						rightMargin: Style.paddingS
						verticalCenter: parent.verticalCenter
					}
					width: Math.max(Style.badgeSizeMin, unreadText.implicitWidth + Style.paddingXS * 2)
					height: Style.badgeSizeMin
					radius: height / 2
					color: Style.accentColor

					Text {
						id: unreadText
						anchors.centerIn: parent
						text: model.unreadCount > 99 ? "99+" : model.unreadCount.toString()
						font.pixelSize: Style.fontSizeXS
						color: "white"
						font.bold: true
					}
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						chatPanelContainer.currentConversationId = model.id;
						chatPanelContainer.conversationSelected(model.id);
					}
				}
			}

			ScrollBar.vertical: ScrollBar {}
		}
	}
}
