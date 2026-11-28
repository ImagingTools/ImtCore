// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtassistgui 1.0

/**
	AssistantPanel

	Embeddable assistant panel: history, input, processing state with cancel, prepared
	changes with confirm/reject, and the context released for the next message.

	The conversation state is kept in the controller. Hiding the panel, or destroying and
	re-creating it with the same controller, keeps the conversation; the panel never touches
	editors of the application.

	Usage:
		AssistantPanel {
			controller: GqlBasedAssistantController {}
		}

	Keyboard: Enter sends the message, Escape cancels the running request.
*/
Rectangle {
	id: assistantPanel
	objectName: "AssistantPanel"

	color: Style.backgroundColor

	property AssistantController controller: null
	property string title: qsTr("Assistant")

	// Show a close button in the header, e.g. in the side panel of the application.
	property bool closable: false

	signal closeRequested()

	/**
		Registry of the components for structured results. The standard list is registered by default.
	*/
	property AssistantRepresentationRegistry representationRegistry: AssistantRepresentationRegistry {}

	readonly property bool busy: assistantPanel.controller !== null && assistantPanel.controller.busy

	Component {
		id: selectableListComp

		AssistantSelectableList {}
	}

	Component.onCompleted: {
		if (assistantPanel.representationRegistry.findComponent("imtassist.selectableList", 1) === null){
			assistantPanel.representationRegistry.registerComponent("imtassist.selectableList", 1, selectableListComp)
		}
	}

	/**
		Release application context for the next message, e.g. the current selection of an editor.
		The context is shown in the panel and can be removed by the user.
	*/
	function addContext(kind, label, data){
		if (assistantPanel.controller !== null){
			assistantPanel.controller.addContextItem(kind, label, data)
		}
	}

	function sendInput(){
		if (assistantPanel.controller === null){
			return
		}

		if (assistantPanel.controller.sendMessage(inputField.text)){
			inputField.text = ""
		}
	}

	function cancelRequest(){
		if (assistantPanel.controller !== null){
			assistantPanel.controller.cancel()
		}
	}

	function focusInput(){
		inputField.forceInputFocus()
	}

	Connections {
		target: assistantPanel.controller

		function onMessagesAppended(){
			messagesView.positionViewAtEnd()
		}
	}

	Rectangle {
		id: header
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		height: Style.controlHeightL
		color: Style.baseColor

		BaseText {
			anchors.left: parent.left
			anchors.leftMargin: Style.marginM
			anchors.verticalCenter: parent.verticalCenter
			text: assistantPanel.title
			font.bold: true
		}

		Button {
			id: newConversationButton
			objectName: "AssistantNewConversation"
			anchors.right: closeButton.visible ? closeButton.left : parent.right
			anchors.rightMargin: Style.marginM
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("New conversation")
			enabled: assistantPanel.controller !== null && !assistantPanel.busy

			onClicked: {
				assistantPanel.controller.newConversation()
				assistantPanel.focusInput()
			}
		}

		Button {
			id: reloadHistoryButton
			objectName: "AssistantReloadHistory"
			anchors.right: newConversationButton.left
			anchors.rightMargin: Style.spacingS
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Reload")
			enabled: assistantPanel.controller !== null && assistantPanel.controller.conversationId !== ""
			visible: enabled

			onClicked: {
				assistantPanel.controller.refreshHistory()
			}
		}

		ToolButton {
			id: closeButton
			objectName: "AssistantClose"
			anchors.right: parent.right
			anchors.rightMargin: Style.marginM
			anchors.verticalCenter: parent.verticalCenter
			width: Style.buttonWidthM
			height: width
			visible: assistantPanel.closable
			tooltipText: qsTr("Close")
			iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

			onClicked: {
				assistantPanel.closeRequested()
			}
		}
	}

	ListView {
		id: messagesView
		objectName: "AssistantMessages"
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: header.bottom
		anchors.bottom: footer.top
		anchors.margins: Style.marginM
		clip: true
		spacing: Style.spacingS
		model: assistantPanel.controller !== null ? assistantPanel.controller.messagesModel : null

		delegate: AssistantMessageDelegate {
			width: messagesView.width
			message: JSON.parse(model.messageJson)
			controller: assistantPanel.controller
			representationRegistry: assistantPanel.representationRegistry

			onSelectionHandedOver: {
				assistantPanel.addContext(contextKind, contextLabel, contextData)
				assistantPanel.focusInput()
			}
		}
	}

	BaseText {
		anchors.centerIn: messagesView
		width: messagesView.width
		horizontalAlignment: Text.AlignHCenter
		visible: messagesView.count === 0
		text: qsTr("Describe a task, for example: \"Show open tasks\".")
		color: Style.inactiveTextColor
		wrapMode: Text.WordWrap
	}

	Column {
		id: footer
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Style.marginM
		spacing: Style.spacingS

		// Context released for the next message
		Flow {
			id: contextFlow
			objectName: "AssistantContext"
			width: parent.width
			spacing: Style.spacingS
			visible: assistantPanel.controller !== null && assistantPanel.controller.contextItems.length > 0

			BaseText {
				height: Style.controlHeightM
				verticalAlignment: Text.AlignVCenter
				text: qsTr("Context:")
				color: Style.inactiveTextColor
			}

			Repeater {
				model: assistantPanel.controller !== null ? assistantPanel.controller.contextItems : []

				Rectangle {
					objectName: "AssistantContextItem"
					height: Style.controlHeightM
					width: contextLabel.width + removeContextButton.width + 3 * Style.marginXS
					radius: Style.radiusS
					color: Style.alternateBaseColor
					border.color: Style.borderColor

					BaseText {
						id: contextLabel
						anchors.left: parent.left
						anchors.leftMargin: Style.marginXS
						anchors.verticalCenter: parent.verticalCenter
						text: modelData.label
						textFormat: Text.PlainText
					}

					Button {
						id: removeContextButton
						objectName: "AssistantRemoveContext"
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXS
						anchors.verticalCenter: parent.verticalCenter
						width: Style.buttonHeightS
						height: Style.buttonHeightS
						text: "×"
						tooltipText: qsTr("Remove from context")

						onClicked: {
							assistantPanel.controller.removeContextItem(index)
						}
					}
				}
			}
		}

		// Processing state
		Row {
			id: busyRow
			objectName: "AssistantBusy"
			spacing: Style.spacingS
			visible: assistantPanel.busy

			BusyIndicator {
				width: Style.controlHeightM
				height: Style.controlHeightM
				visible: assistantPanel.busy
			}

			BaseText {
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Working…")
			}

			Button {
				id: cancelButton
				objectName: "AssistantCancel"
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Cancel")

				onClicked: {
					assistantPanel.cancelRequest()
				}
			}
		}

		// Input
		Row {
			width: parent.width
			spacing: Style.spacingS

			CustomTextField {
				id: inputField
				objectName: "AssistantInput"
				width: parent.width - sendButton.width - Style.spacingS
				height: Style.controlHeightM
				placeHolderText: qsTr("Message to the assistant")
				maximumLength: 8000

				onAccepted: {
					assistantPanel.sendInput()
				}

				Keys.onEscapePressed: {
					assistantPanel.cancelRequest()
				}
			}

			Button {
				id: sendButton
				objectName: "AssistantSend"
				text: qsTr("Send")
				enabled: assistantPanel.controller !== null && !assistantPanel.busy && inputField.text.trim() !== ""

				onClicked: {
					assistantPanel.sendInput()
				}
			}
		}
	}
}
