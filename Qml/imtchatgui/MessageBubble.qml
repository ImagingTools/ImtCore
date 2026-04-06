// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: messageBubbleRoot
	objectName: "MessageBubble"

	property string messageId: ""
	property string senderId: ""
	property string senderName: ""
	property string content: ""
	property string timestamp: ""
	property int status: 0  // 0=Sent, 1=Delivered, 2=Read
	property bool isOwn: false
	property bool hasEntityRefs: false

	signal createTicketRequested(string messageId, string content)

	width: parent ? parent.width : 0
	height: bubbleColumn.implicitHeight + Style.paddingS * 2

	Column {
		id: bubbleColumn
		anchors.left: messageBubbleRoot.isOwn ? undefined : parent.left
		anchors.right: messageBubbleRoot.isOwn ? parent.right : undefined
		anchors.leftMargin: messageBubbleRoot.isOwn ? 0 : Style.paddingM
		anchors.rightMargin: messageBubbleRoot.isOwn ? Style.paddingM : 0
		anchors.top: parent.top
		anchors.topMargin: Style.paddingXS
		spacing: Style.paddingXS
		width: Math.min(parent.width * 0.75, implicitWidth)

		// Sender name (only for received messages)
		Text {
			visible: !messageBubbleRoot.isOwn && messageBubbleRoot.senderName.length > 0
			text: messageBubbleRoot.senderName
			font.pixelSize: Style.fontSizeXS
			font.bold: true
			color: Style.accentColor
		}

		// Message bubble
		Rectangle {
			id: bubble
			width: Math.min(bubbleColumn.width, contentText.implicitWidth + Style.paddingM * 2)
			height: contentText.implicitHeight + Style.paddingS * 2
			radius: Style.radiusS
			color: messageBubbleRoot.isOwn ? Style.accentColor : Style.surfaceColor

			Text {
				id: contentText
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.margins: Style.paddingM
				text: messageBubbleRoot.content
				font.pixelSize: Style.fontSizeS
				color: messageBubbleRoot.isOwn ? "white" : Style.textPrimaryColor
				wrapMode: Text.Wrap
			}

			// Context menu on right-click
			MouseArea {
				anchors.fill: parent
				acceptedButtons: Qt.RightButton
				onClicked: {
					if (mouse.button === Qt.RightButton) {
						contextMenu.popup();
					}
				}
			}

			// Menu {
			// 	id: contextMenu

			// 	MenuItem {
			// 		text: qsTr("Create Ticket")
			// 		onTriggered: {
			// 			messageBubbleRoot.createTicketRequested(
			// 				messageBubbleRoot.messageId,
			// 				messageBubbleRoot.content
			// 			);
			// 		}
			// 	}

			// 	MenuItem {
			// 		text: qsTr("Copy Text")
			// 		onTriggered: {
			// 			clipboard.setText(messageBubbleRoot.content);
			// 		}
			// 	}
			// }
		}

		// Footer row: timestamp + status indicator
		Row {
			anchors.right: messageBubbleRoot.isOwn ? bubbleColumn.right : undefined
			anchors.left: messageBubbleRoot.isOwn ? undefined : bubbleColumn.left
			spacing: Style.paddingXS

			Text {
				text: messageBubbleRoot.timestamp
				font.pixelSize: Style.fontSizeXS
				color: Style.textSecondaryColor
				anchors.verticalCenter: parent.verticalCenter
			}

			// Status checkmarks (own messages only)
			Text {
				visible: messageBubbleRoot.isOwn
				text: messageBubbleRoot.status === 2 ? "✓✓"
					: messageBubbleRoot.status === 1 ? "✓✓"
					: "✓"
				font.pixelSize: Style.fontSizeXS
				color: messageBubbleRoot.status === 2 ? Style.accentColor : Style.textSecondaryColor
				anchors.verticalCenter: parent.verticalCenter
			}
		}
	}
}
