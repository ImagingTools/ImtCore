// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0

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
	property var reactions: []

	signal createTicketRequested(string messageId, string content)
	signal reactionAdded(string messageId, string reaction)

	width: parent ? parent.width : 0
	height: bubbleColumn.height + Style.paddingS * 2

	Column {
		id: bubbleColumn
		anchors.left: messageBubbleRoot.isOwn ? undefined : parent.left
		anchors.right: messageBubbleRoot.isOwn ? parent.right : undefined
		anchors.leftMargin: messageBubbleRoot.isOwn ? 0 : Style.paddingM
		anchors.rightMargin: messageBubbleRoot.isOwn ? Style.paddingM : 0
		anchors.top: parent.top
		anchors.topMargin: Style.paddingS
		spacing: Style.paddingS
		width: parent.width * 0.75

		// Sender name (only for received messages)
		Text {
			visible: !messageBubbleRoot.isOwn && messageBubbleRoot.senderName.length > 0
			text: messageBubbleRoot.senderName
			font.pixelSize: Style.fontSizeXS
			font.bold: true
			color: Style.imaginToolsAccentColor
		}

		// Message bubble
		Rectangle {
			id: bubble
			width: bubbleColumn.width
			height: contentText.height + Style.paddingS * 2
			radius: Style.radiusS
			color: messageBubbleRoot.isOwn ? Style.imaginToolsAccentColor : Style.baseColor

			Text {
				id: contentText
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.margins: Style.paddingM
				text: messageBubbleRoot.content
				font.pixelSize: Style.fontSizeS
				color: messageBubbleRoot.isOwn ? "white" : Style.imaginToolsAccentColor
				wrapMode: Text.Wrap
			}
		}

		// Reactions row (like GitHub comment reactions: 👍 👎 ❤️ 🚀 etc.)
		Row {
			visible: messageBubbleRoot.reactions.length > 0
			spacing: Style.paddingS

			Repeater {
				model: messageBubbleRoot.reactions

				Rectangle {
					height: 24
					width: reactionText.width + Style.paddingS * 2
					radius: height / 2
					color: Style.baseColor
					border.color: Style.borderColor2

					Text {
						id: reactionText
						anchors.centerIn: parent
						text: modelData
						font.pixelSize: Style.fontSizeXS
					}
				}
			}
		}

		// Footer row: timestamp + status indicator
		Row {
			anchors.right: messageBubbleRoot.isOwn ? bubbleColumn.right : undefined
			anchors.left: messageBubbleRoot.isOwn ? undefined : bubbleColumn.left
			spacing: Style.paddingS

			Text {
				text: messageBubbleRoot.timestamp
				font.pixelSize: Style.fontSizeXS
				color: Style.buttonInactiveTextColor
				anchors.verticalCenter: parent.verticalCenter
			}

			// Status checkmarks (own messages only)
			Text {
				visible: messageBubbleRoot.isOwn
				text: messageBubbleRoot.status === 2 ? "✓✓"
					: messageBubbleRoot.status === 1 ? "✓✓"
					: "✓"
				font.pixelSize: Style.fontSizeXS
				color: messageBubbleRoot.status === 2 ? Style.imaginToolsAccentColor : Style.buttonInactiveTextColor
				anchors.verticalCenter: parent.verticalCenter
			}
		}
	}
}
