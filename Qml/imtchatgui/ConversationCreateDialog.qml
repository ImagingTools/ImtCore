// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: conversationCreateDialogRoot
	objectName: "ConversationCreateDialog"
	visible: false
	color: "transparent"
	anchors.fill: parent

	signal conversationCreated(var convData)
	signal cancelled()

	// Backdrop
	Rectangle {
		anchors.fill: parent
		color: "black"
		opacity: 0.45

		MouseArea {
			anchors.fill: parent
			onClicked: {
				conversationCreateDialogRoot.cancel()
			}
		}
	}

	// Dialog card
	Rectangle {
		id: dialogCard
		anchors.centerIn: parent
		width: Math.min(parent.width * 0.9, Style.sizeHintL)
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

			Text {
				text: qsTr("New Conversation")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.imaginToolsAccentColor
			}

			// Name field
			Column {
				width: parent.width
				spacing: Style.paddingS

				Text {
					text: qsTr("Name *")
					font.pixelSize: Style.fontSizeXS
					color: Style.buttonInactiveTextColor
				}

				Rectangle {
					width: parent.width
					radius: Style.radiusS
					border.color: nameField.activeFocus ? Style.imaginToolsAccentColor : Style.borderColor2

					TextInput {
						id: nameField
						anchors.fill: parent
						anchors.margins: Style.paddingS
						font.pixelSize: Style.fontSizeS
						color: Style.imaginToolsAccentColor
						clip: true
						verticalAlignment: TextInput.AlignVCenter

						Text {
							anchors.fill: parent
							text: qsTr("Conversation name")
							font.pixelSize: Style.fontSizeS
							verticalAlignment: Text.AlignVCenter
							visible: nameField.text.length === 0
						}
					}
				}
			}

			// Type selection
			Column {
				width: parent.width
				spacing: Style.paddingS

				Text {
					text: qsTr("Type")
					font.pixelSize: Style.fontSizeXS
					color: Style.buttonInactiveTextColor
				}

				ComboBox {
					id: typeCombo
					width: parent.width
					model: [
						qsTr("Direct (1:1)"),
						qsTr("Group"),
						qsTr("Channel"),
						qsTr("Support")
					]
					currentIndex: 0
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
					border.color: Style.borderColor2

					Text {
						anchors.centerIn: parent
						text: qsTr("Cancel")
						font.pixelSize: Style.fontSizeS
						color: Style.imaginToolsAccentColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							conversationCreateDialogRoot.cancel()
						}
					}
				}

				Rectangle {
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS

					Text {
						anchors.centerIn: parent
						text: qsTr("Create")
						font.pixelSize: Style.fontSizeS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						enabled: nameField.text.trim().length > 0
						onClicked: {
							conversationCreateDialogRoot.submit()
						}
					}
				}
			}
		}
	}

	function open() {
		visible = true;
		nameField.text = "";
		typeCombo.currentIndex = 0;
		nameField.forceActiveFocus();
	}

	function cancel() {
		visible = false;
		cancelled();
	}

	function submit() {
		let convData = {
			name: nameField.text.trim(),
			conversationType: typeCombo.currentIndex,
			participantIds: []
		};
		conversationCreated(convData);
		visible = false;
	}
}
