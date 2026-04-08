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
				spacing: Style.paddingXS

				Text {
					text: qsTr("Title *")
					font.pixelSize: Style.fontSizeXS
					color: Style.textSecondaryColor
				}

				Rectangle {
					width: parent.width
					height: Style.inputHeightM
					radius: Style.radiusS
					color: Style.inputBackgroundColor
					border.color: titleField.activeFocus ? Style.accentColor : Style.separatorColor

					TextInput {
						id: titleField
						anchors.fill: parent
						anchors.margins: Style.paddingS
						font.pixelSize: Style.fontSizeS
						color: Style.textPrimaryColor
						clip: true
						verticalAlignment: TextInput.AlignVCenter

						Text {
							anchors.fill: parent
							text: qsTr("Brief summary of the issue")
							color: Style.textPlaceholderColor
							font.pixelSize: Style.fontSizeS
							verticalAlignment: Text.AlignVCenter
							visible: titleField.text.length === 0
						}
					}
				}
			}

			// Description field
			Column {
				width: parent.width
				spacing: Style.paddingXS

				Text {
					text: qsTr("Description")
					font.pixelSize: Style.fontSizeXS
					color: Style.textSecondaryColor
				}

				Rectangle {
					width: parent.width
					height: 100
					radius: Style.radiusS
					color: Style.inputBackgroundColor
					border.color: descriptionField.activeFocus ? Style.accentColor : Style.separatorColor

					TextEdit {
						id: descriptionField
						anchors.fill: parent
						anchors.margins: Style.paddingS
						font.pixelSize: Style.fontSizeS
						color: Style.textPrimaryColor
						wrapMode: TextEdit.Wrap
						text: ticketCreateDialogRoot.prefillContent

						Text {
							anchors.fill: parent
							text: qsTr("Add a description...")
							color: Style.textPlaceholderColor
							font.pixelSize: Style.fontSizeS
							visible: descriptionField.text.length === 0
						}
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
					color: titleField.text.trim().length > 0 ? "#1a7f37" : Style.disabledColor

					Text {
						anchors.centerIn: parent
						text: qsTr("Submit new issue")
						font.pixelSize: Style.fontSizeS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						enabled: titleField.text.trim().length > 0
						onClicked: {
							ticketCreateDialogRoot.submitTicket()
						}
					}
				}
			}
		}
	}

	function open(prefill, messageId, conversationId) {
		prefillContent = prefill || "";
		linkedMessageId = messageId || "";
		linkedConversationId = conversationId || "";
		visible = true;
		titleField.forceActiveFocus();
	}

	function cancel() {
		visible = false;
		titleField.text = "";
		descriptionField.text = "";
		cancelled();
	}

	function submitTicket() {
		let ticketData = {
			title: titleField.text.trim(),
			description: descriptionField.text.trim(),
			ticketType: typeCombo.currentIndex,
			priority: priorityCombo.currentIndex,
			messageId: linkedMessageId,
			conversationId: linkedConversationId
		};

		ticketCreated(ticketData);
		visible = false;
		titleField.text = "";
		descriptionField.text = "";
	}
}
