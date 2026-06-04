// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: messageInputRoot
	objectName: "MessageInput"
	color: Style.surfaceColor
	height: inputRow.height + Style.paddingS * 2

	property string conversationId: ""
	property var pendingEntityRefs: []

	signal messageReady(string content, var entityRefs, var attachmentIds)
	signal typingStarted()
	signal typingStopped()

	Column {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.paddingS
		spacing: Style.paddingS

		// Entity reference chips row
		Row {
			id: entityRefChips
			width: parent.width
			spacing: Style.paddingS
			visible: messageInputRoot.pendingEntityRefs.length > 0

			Repeater {
				model: messageInputRoot.pendingEntityRefs

				EntityReferenceCard {
					entityType: modelData.entityType || ""
					entityId: modelData.entityId || ""
					displayName: modelData.displayName || ""
					compact: true

					onRemoveRequested: {
					}
				}
			}
		}

		// Input row
		Row {
			id: inputRow
			width: parent.width
			spacing: Style.paddingS

			TextInput {
				id: textField
				width: parent.width - sendButton.width - Style.paddingS
				height: Style.inputHeightM
				verticalAlignment: TextInput.AlignVCenter
				font.pixelSize: Style.fontSizeS
				color: Style.imaginToolsAccentColor
				selectionColor: Style.imaginToolsAccentColor
				clip: true

				property bool _typing: false
				property var _typingTimer: null

				Text {
					anchors.fill: parent
					anchors.leftMargin: Style.paddingS
					text: qsTr("Write a message... (@ to mention, # for reference)")
					color: Style.textPlaceholderColor
					font.pixelSize: Style.fontSizeS
					verticalAlignment: Text.AlignVCenter
					visible: textField.text.length === 0
				}

				Keys.onReturnPressed: {
					if (event.modifiers & Qt.ShiftModifier) {
						// Shift+Enter inserts newline (future multiline support)
					} else {
						messageInputRoot.sendMessage();
					}
				}

				onTextChanged: {
					if (text.length > 0 && !_typing) {
						_typing = true;
						messageInputRoot.typingStarted();
					}

					// Reset typing timer
					if (_typingTimer) {
						_typingTimer.restart();
					} else {
						_typingTimer = typingTimerComponent.createObject(textField);
					}

					// Check for @ or # trigger
					let cursor = text.lastIndexOf('@', textField.cursorPosition - 1);
					let hashCursor = text.lastIndexOf('#', textField.cursorPosition - 1);
					let triggerPos = Math.max(cursor, hashCursor);
					if (triggerPos >= 0 && textField.cursorPosition - triggerPos <= 20) {
						entitySearchPopup.searchQuery = text.substring(triggerPos + 1, textField.cursorPosition);
						entitySearchPopup.triggerChar = text[triggerPos];
						entitySearchPopup.triggerPos = triggerPos;
						entitySearchPopup.open();
					} else {
						entitySearchPopup.close();
					}
				}

				Component {
					id: typingTimerComponent
					Timer {
						interval: 2000
						onTriggered: {
							textField._typing = false;
							messageInputRoot.typingStopped();
						}
					}
				}
			}

			// Send button
			Rectangle {
				id: sendButton
				width: Style.buttonHeightM
				radius: Style.radiusS
				color: textField.text.trim().length > 0 ? Style.imaginToolsAccentColor : Style.disabledColor

				Text {
					anchors.centerIn: parent
					text: "➤"
					font.pixelSize: Style.fontSizeM
					color: "white"
				}

				MouseArea {
					anchors.fill: parent
					enabled: textField.text.trim().length > 0
					onClicked: {
						messageInputRoot.sendMessage()
					}
				}
			}
		}
	}

	// Entity search popup
	EntitySearchPopup {
		id: entitySearchPopup
		anchors.bottom: messageInputRoot.top
		anchors.left: messageInputRoot.left
		width: messageInputRoot.width * 0.6

		onEntitySelected: {
			let refs = messageInputRoot.pendingEntityRefs.slice();
			refs.push({ entityType: entityType, entityId: entityId, displayName: displayName });
			messageInputRoot.pendingEntityRefs = refs;

			// Remove the trigger text
			let before = textField.text.substring(0, entitySearchPopup.triggerPos);
			let after = textField.text.substring(textField.cursorPosition);
			textField.text = before + after;
			entitySearchPopup.close();
		}
	}

	function sendMessage() {
		let trimmed = textField.text.trim();
		if (trimmed.length === 0) {
			return;
		}

		messageInputRoot.messageReady(trimmed, pendingEntityRefs, []);
		textField.text = "";
		pendingEntityRefs = [];
	}
}
