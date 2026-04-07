// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: chatViewContainer
	objectName: "ChatView"
	color: Style.backgroundColor

	property string conversationId: ""
	property string conversationName: ""
	property var messagesModel: null
	property bool isTyping: false
	property string typingUserName: ""

	signal messageSent(string content, var entityRefs, var attachmentIds)
	signal ticketCreateRequested(string messageId, string content)

	Column {
		anchors.fill: parent
		spacing: 0

		// Conversation header
		Rectangle {
			id: chatHeader
			width: parent.width
			height: Style.sizeS
			color: Style.panelHeaderColor

			Row {
				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
					leftMargin: Style.paddingM
					rightMargin: Style.paddingM
				}
				spacing: Style.paddingS

				Rectangle {
					width: Style.avatarSizeS
					height: Style.avatarSizeS
					radius: Style.avatarSizeS / 2
					color: Style.accentColor
					anchors.verticalCenter: parent.verticalCenter

					Text {
						anchors.centerIn: parent
						text: chatViewContainer.conversationName.charAt(0).toUpperCase()
						font.pixelSize: Style.fontSizeS
						color: "white"
						font.bold: true
					}
				}

				Text {
					text: chatViewContainer.conversationName
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textPrimaryColor
					anchors.verticalCenter: parent.verticalCenter
				}
			}
		}

		// Messages area
		ListView {
			id: messagesListView
			width: parent.width
			height: parent.height - chatHeader.height - messageInput.height
			clip: true
			model: chatViewContainer.messagesModel
			spacing: Style.paddingXS
			verticalLayoutDirection: ListView.BottomToTop

			delegate: MessageBubble {
				messageId: model.id || ""
				senderId: model.senderId || ""
				senderName: model.senderName || ""
				content: model.content || model.contentPreview || ""
				timestamp: model.createdAt || ""
				status: model.status || 0
				isOwn: model.isOwn || false
				hasEntityRefs: model.hasEntityRefs || false

				onCreateTicketRequested: function(msgId, msgContent) {
					chatViewContainer.ticketCreateRequested(msgId, msgContent);
				}
			}

			// Typing indicator shown at the top (bottom of view due to BottomToTop)
			footer: TypingIndicator {
				visible: chatViewContainer.isTyping
				width: messagesListView.width
				userName: chatViewContainer.typingUserName
			}

			ScrollBar.vertical: ScrollBar {}

			Component.onCompleted: {
				positionViewAtEnd();
			}

			onCountChanged: {
				Qt.callLater(positionViewAtEnd);
			}
		}

		// Message input area
		MessageInput {
			id: messageInput
			width: parent.width
			conversationId: chatViewContainer.conversationId

			onMessageReady: function(content, entityRefs, attachmentIds) {
				chatViewContainer.messageSent(content, entityRefs, attachmentIds);
			}
		}
	}

	// Notification banner overlay
	ChatNotificationBanner {
		id: notificationBanner
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}
	}

	function showNotification(senderName, preview) {
		notificationBanner.show(senderName, preview);
	}
}
