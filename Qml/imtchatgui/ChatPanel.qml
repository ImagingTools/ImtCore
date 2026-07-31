// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0

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
			height: Style.buttonHeightS
			color: Style.borderColor2

			Row {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: Style.paddingM
				anchors.rightMargin: Style.paddingS

				Text {
					text: qsTr("Chats")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.imaginToolsAccentColor
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
						color: Style.imaginToolsAccentColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							chatPanelContainer.newConversationRequested()
						}
					}
				}
			}
		}

		// Search bar
		ChatSearchBar {
			id: searchBar
			width: parent.width
			height: Style.buttonHeightM
			placeholderText: qsTr("Search conversations...")
		}

		// Conversation list
		ListView {
			id: conversationListView
			width: parent.width
			height: parent.height - Style.buttonHeightS - searchBar.height
			clip: true
			model: chatPanelContainer.conversationsModel

			delegate: Rectangle {
				width: conversationListView.width
				color: model.id === chatPanelContainer.currentConversationId
					? Style.selectedColor
					: "transparent"

				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: 1
					color: Style.borderColor2
					opacity: 0.5
				}

				Row {
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.leftMargin: Style.paddingM
					anchors.rightMargin: Style.paddingS
					spacing: Style.paddingS

					// Avatar placeholder
					Rectangle {
						width: Style.controlHeightL
						height: Style.controlHeightL
						radius: Style.controlHeightL / 2
						color: Style.imaginToolsAccentColor
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
						width: parent.width - Style.controlHeightL - Style.paddingS

						Text {
							text: model.name || ""
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.imaginToolsAccentColor
							elide: Text.ElideRight
							width: parent.width
						}

						Text {
							text: model.lastMessage || ""
							font.pixelSize: Style.fontSizeXS
							color: Style.buttonInactiveTextColor
							elide: Text.ElideRight
							width: parent.width
						}
					}
				}

				// Unread badge
				Rectangle {
					visible: model.unreadCount > 0
					anchors.right: parent.right
					anchors.rightMargin: Style.paddingS
					anchors.verticalCenter: parent.verticalCenter
					width: Style.sizeHintL
					height: Style.sizeHintL
					radius: height / 2
					color: Style.imaginToolsAccentColor

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
		}
	}
}
