// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
	AssistantMessageDelegate

	Shows one validated assistant message. Structured results are rendered only by a
	component registered for their type and version; an unknown type is shown as an error.
*/
Item {
	id: messageDelegate
	objectName: "AssistantMessageDelegate"

	width: parent ? parent.width : 0
	height: messageColumn.height + Style.marginS

	property var message: ({})
	property var controller: null
	property var representationRegistry: null

	property string kind: typeof message.kind === "string" ? message.kind : ""
	property string text: typeof message.text === "string" ? message.text : ""
	property string code: typeof message.code === "string" ? message.code : ""
	property bool isOwn: kind === "userText"
	property bool isProblem: kind === "error" || (kind === "actionExecuted" && code !== "succeeded")
	property bool representationFailed: false

	signal selectionHandedOver(string contextKind, string contextLabel, var contextData)

	function errorText(code){
		if (code === "AssistantDisabled"){
			return qsTr("The assistant is not enabled.")
		}
		if (code === "ProviderNotConfigured" || code === "ProviderError"){
			return qsTr("The assistant is currently not available.")
		}
		if (code === "InvalidContext"){
			return qsTr("You are not signed in.")
		}
		if (code === "InvalidInput"){
			return qsTr("The message is empty or too long.")
		}
		if (code === "ConversationNotFound"){
			return qsTr("The conversation no longer exists. Start a new one.")
		}
		if (code === "Busy"){
			return qsTr("The previous message is still being processed.")
		}
		if (code === "Timeout"){
			return qsTr("The assistant did not answer in time.")
		}
		if (code === "Canceled"){
			return qsTr("Cancelled. A change that was already executed is not undone.")
		}
		if (code === "InvalidResponse" || code === "InvalidMessage"){
			return qsTr("The assistant returned an invalid answer.")
		}
		if (code === "UnknownAction"){
			return qsTr("The assistant requested a function that is not available.")
		}
		if (code === "InvalidParameters"){
			return qsTr("The assistant requested a function with invalid input.")
		}
		if (code === "AccessDenied"){
			return qsTr("You are not allowed to do this.")
		}
		if (code === "InvalidRepresentation"){
			return qsTr("The result cannot be displayed.")
		}
		if (code === "ActionFailed"){
			return qsTr("The function could not be executed.")
		}
		if (code === "TooManySteps"){
			return qsTr("The request could not be completed.")
		}
		if (code === "PreviewNotFound"){
			return qsTr("The proposed change does not exist.")
		}
		if (code === "PreviewExpired"){
			return qsTr("The proposed change has expired. Ask again to prepare it anew.")
		}
		if (code === "PreviewRejected"){
			return qsTr("The proposed change was rejected.")
		}
		if (code === "PreviewInProgress"){
			return qsTr("The proposed change is being executed.")
		}
		if (code === "NetworkError"){
			return qsTr("The server could not be reached.")
		}
		if (code === "ConfirmationStatusUnknown"){
			return qsTr("The answer to the confirmation was lost. Reload the conversation to see whether the change was executed.")
		}
		if (code === "CancelFailed"){
			return qsTr("The request could not be canceled.")
		}
		if (code === "HistoryReloadFailed"){
			return qsTr("The conversation could not be reloaded.")
		}

		return qsTr("An error occurred.")
	}

	function executedText(code){
		if (code === "succeeded"){
			return qsTr("Change executed.")
		}
		if (code === "stale"){
			return qsTr("Not executed: the object was changed in the meantime. Ask again to prepare a new proposal.")
		}
		if (code === "accessDenied"){
			return qsTr("Not executed: you are not allowed to make this change.")
		}

		return qsTr("Not executed: the change failed.")
	}

	function statusText(code){
		if (code === "previewRejected"){
			return qsTr("The proposed change was rejected. Nothing was changed.")
		}
		if (code === "alreadyProcessed"){
			return qsTr("This change was already processed.")
		}
		if (code === "rejectionStatusUnknown"){
			return qsTr("The answer to the rejection was lost.")
		}

		return code
	}

	function displayText(){
		if (messageDelegate.kind === "error"){
			return messageDelegate.errorText(messageDelegate.code)
		}

		if (messageDelegate.kind === "actionExecuted"){
			let result = messageDelegate.executedText(messageDelegate.code)
			if (messageDelegate.code === "succeeded" && messageDelegate.text !== ""){
				result = result + " " + messageDelegate.text
			}

			return result
		}

		if (messageDelegate.kind === "status"){
			return messageDelegate.statusText(messageDelegate.code)
		}

		return messageDelegate.text
	}

	function representationComponent(){
		let representation = messageDelegate.message.representation
		if (messageDelegate.kind !== "actionResult" && messageDelegate.kind !== "actionExecuted"){
			return null
		}

		if (representation === undefined || representation === null || messageDelegate.representationRegistry === null){
			return null
		}

		return messageDelegate.representationRegistry.findComponent(representation.type, representation.version)
	}

	function hasRepresentation(){
		let representation = messageDelegate.message.representation

		return representation !== undefined && representation !== null && typeof representation.type === "string"
	}

	Column {
		id: messageColumn
		anchors.left: messageDelegate.isOwn ? undefined : parent.left
		anchors.right: messageDelegate.isOwn ? parent.right : undefined
		width: messageDelegate.isOwn ? parent.width * 0.8 : parent.width
		spacing: Style.spacingS

		Rectangle {
			id: bubble
			width: parent.width
			height: bubbleText.height + 2 * Style.marginS
			visible: bubbleText.text !== ""
			radius: Style.radiusS
			color: messageDelegate.isOwn ? Style.imagingToolsAccentColor : Style.baseColor
			border.color: messageDelegate.isProblem ? Style.errorTextColor : "transparent"

			BaseText {
				id: bubbleText
				objectName: "AssistantMessageText"
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.margins: Style.marginS
				text: messageDelegate.displayText()
				textFormat: Text.PlainText
				wrapMode: Text.WordWrap
				color: messageDelegate.isOwn ? Style.highlightedTextColor : (messageDelegate.isProblem ? Style.errorTextColor : Style.textColor)
			}
		}

		Flow {
			id: optionsFlow
			width: parent.width
			spacing: Style.spacingS
			visible: messageDelegate.kind === "clarification" && Array.isArray(messageDelegate.message.options)

			Repeater {
				model: Array.isArray(messageDelegate.message.options) ? messageDelegate.message.options : []

				Button {
					objectName: "AssistantClarificationOption"
					text: modelData
					enabled: messageDelegate.controller !== null && !messageDelegate.controller.busy

					onClicked: {
						messageDelegate.controller.sendMessage(modelData)
					}
				}
			}
		}

		AssistantPreviewCard {
			id: previewCard
			width: parent.width
			visible: messageDelegate.kind === "actionPreview"
			previewId: typeof messageDelegate.message.previewId === "string" ? messageDelegate.message.previewId : ""
			summary: messageDelegate.text
			targetLabel: messageDelegate.message.data && typeof messageDelegate.message.data.targetLabel === "string" ? messageDelegate.message.data.targetLabel : ""
			changes: messageDelegate.message.data && Array.isArray(messageDelegate.message.data.changes) ? messageDelegate.message.data.changes : []
			impact: messageDelegate.message.data && typeof messageDelegate.message.data.impact === "string" ? messageDelegate.message.data.impact : ""
			decisionState: messageDelegate.controller !== null ? messageDelegate.controller.getPreviewState(previewCard.previewId) : "decided"

			onConfirmClicked: {
				messageDelegate.controller.confirmPreview(previewCard.previewId)
			}

			onRejectClicked: {
				messageDelegate.controller.rejectPreview(previewCard.previewId)
			}
		}

		Loader {
			id: representationLoader
			width: parent.width
			sourceComponent: messageDelegate.representationComponent()
			visible: status === Loader.Ready && !messageDelegate.representationFailed

			onLoaded: {
				let isValid = representationLoader.item.setRepresentationData(messageDelegate.message.representation.data)
				messageDelegate.representationFailed = !isValid
				if (isValid && representationLoader.item.selectionHandedOver !== undefined){
					representationLoader.item.selectionHandedOver.connect(messageDelegate.selectionHandedOver)
				}
			}
		}

		BaseText {
			id: representationError
			objectName: "AssistantRepresentationError"
			width: parent.width
			visible: messageDelegate.hasRepresentation() && (messageDelegate.representationComponent() === null || messageDelegate.representationFailed)
			text: qsTr("The result cannot be displayed.")
			color: Style.errorTextColor
			wrapMode: Text.WordWrap
		}
	}
}
