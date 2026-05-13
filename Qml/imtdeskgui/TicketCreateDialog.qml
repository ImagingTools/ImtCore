// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: ticketCreateDialogRoot
	objectName: "TicketCreateDialog"
	visible: false
	color: "transparent"

	property string prefillContent: ""
	property string linkedMessageId: ""
	property string linkedConversationId: ""

	signal ticketCreated(var ticketData)
	signal cancelled()

	anchors.fill: parent

	// Backdrop
	Rectangle {
		anchors.fill: parent
		color: "black"
		opacity: 0.45

		MouseArea {
			anchors.fill: parent
			onClicked: {
				ticketCreateDialogRoot.cancel()
			}
		}
	}

	// Dialog card
	Rectangle {
		id: dialogCard
		anchors.centerIn: parent
		width: Math.min(parent.width * 0.9, Style.dialogWidthM)
		height: dialogColumn.height + Style.paddingL * 2
		radius: Style.radiusM
		color: Style.backgroundColor

		Column {
			id: dialogColumn
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.margins: Style.paddingL
			spacing: Style.paddingM

			// Title
			Text {
				text: qsTr("New Issue")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textPrimaryColor
			}

			// Title field
			Column {
				width: parent.width
				spacing: 4

				Text {
					text: qsTr("Title *")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textColor
				}

				CustomTextField {
					id: titleInput
					width: parent.width
					height: Style.controlHeightM
					placeHolderText: qsTr("Brief summary of the issue")
				}
			}

			// Description field
			Column {
				width: parent.width
				spacing: 4

				Text {
					text: qsTr("Description")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textColor
				}

				Rectangle {
					width: parent.width
					height: Math.min(180, Math.max(100, descriptionInput.contentHeight)) + Style.paddingM * 2
					radius: Style.radiusM
					border.color: descriptionInput.activeFocus ? Style.imaginToolsAccentColor : Style.borderColor
					border.width: descriptionInput.activeFocus ? 2 : 1
					color: descriptionInput.activeFocus ? Style.baseColor : "#FAFBFC"

					Flickable {
						id: descFlick
						anchors.fill: parent
						anchors.margins: Style.paddingM
						anchors.rightMargin: Style.paddingM + Style.marginM
						contentWidth: width
						contentHeight: descriptionInput.height
						clip: true
						boundsBehavior: Flickable.StopAtBounds

						TextEdit {
							id: descriptionInput
							width: descFlick.width
							height: Math.max(100, contentHeight)
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
							wrapMode: TextEdit.Wrap
							textFormat: TextEdit.PlainText

							onCursorRectangleChanged: {
								var cy = cursorRectangle.y
								var ch = cursorRectangle.height
								if (cy < descFlick.contentY) {
									descFlick.contentY = cy
								} else if (cy + ch > descFlick.contentY + descFlick.height) {
									descFlick.contentY = cy + ch - descFlick.height
								}
							}

							Text {
								anchors.fill: parent
								text: qsTr("Add a description...")
								color: Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
								visible: descriptionInput.text.length === 0
							}
						}
					}

					CustomScrollbar {
						z: parent.z + 1
						anchors.right: parent.right
						anchors.rightMargin: 2
						anchors.top: parent.top
						anchors.bottom: parent.bottom
						anchors.topMargin: 2
						anchors.bottomMargin: 2
						secondSize: Style.marginM
						targetItem: descFlick
						visible: descFlick.contentHeight > descFlick.height
					}
				}
			}

			// Type + Priority row
			Row {
				width: parent.width
				spacing: Style.paddingM

				Column {
					width: (parent.width - Style.paddingM) / 2
					spacing: Style.paddingXS

					Text {
						text: qsTr("Type")
						font.pixelSize: Style.fontSizeXS
						color: Style.textSecondaryColor
					}

					ComboBox {
						id: typeCombo
						width: parent.width
						currentIndex: 1
					}
				}

				Column {
					width: (parent.width - Style.paddingM) / 2
					spacing: Style.paddingXS

					Text {
						text: qsTr("Priority")
						font.pixelSize: Style.fontSizeXS
						color: Style.textSecondaryColor
					}

					ComboBox {
						id: priorityCombo
						width: parent.width
						currentIndex: 1
					}
				}
			}

			// Buttons
			Row {
				anchors.right: parent.right
				spacing: Style.paddingS

				Rectangle {
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: "transparent"
					border.color: Style.separatorColor

					Text {
						anchors.centerIn: parent
						text: qsTr("Cancel")
						font.pixelSize: Style.fontSizeS
						color: Style.textPrimaryColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketCreateDialogRoot.cancel()
						}
					}
				}

				Rectangle {
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: titleInput.text.trim().length > 0 ? "#1a7f37" : Style.disabledColor

					Text {
						anchors.centerIn: parent
						text: qsTr("Submit new issue")
						font.pixelSize: Style.fontSizeS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						enabled: titleInput.text.trim().length > 0
						onClicked: {
							ticketCreateDialogRoot.submitTicket()
						}
					}
				}
			}
		}
	}

	function open(prefill, messageId, conversationId) {
		ticketCreateDialogRoot.prefillContent = prefill || "";
		ticketCreateDialogRoot.linkedMessageId = messageId || "";
		ticketCreateDialogRoot.linkedConversationId = conversationId || "";
		descriptionInput.text = ticketCreateDialogRoot.prefillContent;
		ticketCreateDialogRoot.visible = true;
		titleInput.forceActiveFocus();
	}

	function cancel() {
		visible = false;
		titleInput.text = "";
		descriptionInput.text = "";
		cancelled();
	}

	function submitTicket() {
		let ticketData = {
			title: titleInput.text.trim(),
			description: descriptionInput.text.trim(),
			ticketType: typeCombo.currentIndex,
			priority: priorityCombo.currentIndex,
			messageId: linkedMessageId,
			conversationId: linkedConversationId
		};

		ticketCreated(ticketData);
		visible = false;
		titleInput.text = "";
		descriptionInput.text = "";
	}
}
