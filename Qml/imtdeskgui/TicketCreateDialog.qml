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

			TicketTitleDescriptionFields {
				id: ticketFields
				width: parent.width
				titleLabelText: qsTr("Title *")
				descriptionLabelText: qsTr("Description")
				titlePlaceholderText: qsTr("Brief summary of the issue")
				descriptionPlaceholderText: qsTr("Add a description...")
				minDescriptionHeight: 100
				maxDescriptionHeight: 180
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
					color: ticketFields.titleText.trim().length > 0 ? "#1a7f37" : Style.disabledColor

					Text {
						anchors.centerIn: parent
						text: qsTr("Submit new issue")
						font.pixelSize: Style.fontSizeS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						enabled: ticketFields.titleText.trim().length > 0
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
		ticketFields.descriptionText = prefillContent
		visible = true;
		ticketFields.focusTitle();
	}

	function cancel() {
		visible = false;
		ticketFields.clearFields()
		cancelled();
	}

	function submitTicket() {
		let ticketData = {
			title: ticketFields.titleText.trim(),
			description: ticketFields.descriptionText.trim(),
			ticketType: typeCombo.currentIndex,
			priority: priorityCombo.currentIndex,
			messageId: linkedMessageId,
			conversationId: linkedConversationId
		};

		ticketCreated(ticketData);
		visible = false;
		ticketFields.clearFields()
	}
}
