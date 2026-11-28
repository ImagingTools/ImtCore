// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15

/**
	AssistantController

	Transport-independent state of an assistant conversation: messages, processing
	state, released context and the decisions on prepared changes. The panel only
	talks to this contract; GqlBasedAssistantController implements the transport.

	The state lives in the controller, so hiding or re-creating the panel keeps the
	conversation and does not touch the editors of the application.

	Messages from the server are untrusted: only messages with a known kind and the
	supported format version are shown, everything else is replaced by an error.
	Success of a change is only shown for an "actionExecuted" message from the server.
*/
QtObject {
	id: root

	property string conversationId: ""
	property int conversationGeneration: 0
	// Whether the assistant is enabled for the organization of the session (AssistantGetStatus).
	property bool available: false
	property bool busy: false
	property string busyCommand: ""
	readonly property int formatVersion: 1

	/**
		Messages of the conversation; role "messageJson" holds the validated message.
	*/
	property ListModel messagesModel: ListModel {}

	/**
		Context released by the user for the next message: [{kind, label, data}].
	*/
	property var contextItems: []

	/**
		Decisions on prepared changes: previewId -> "confirming", "rejecting", "decided".
	*/
	property var previewStates: ({})

	property string __pendingText: ""

	readonly property var knownKinds: ["userText", "assistantText", "clarification", "actionResult", "actionPreview", "actionExecuted", "status", "error"]

	signal messagesAppended()
	signal availabilityChecked(bool available)

	// Transport, implemented by a derived controller.
	function sendRequest(commandId, input){
		console.warn("AssistantController: no transport for", commandId)
		root.handleError(commandId, "")
	}

	/**
		Ask the server whether the assistant is enabled for the organization of the session.
	*/
	function checkAvailability(){
		root.sendRequest("AssistantGetStatus", {})
	}

	function handleStatus(isAvailable){
		root.available = isAvailable === true
		root.availabilityChecked(root.available)
	}

	function startConversation(){
		root.busy = true
		root.busyCommand = "AssistantStartConversation"
		root.sendRequest("AssistantStartConversation", {})
	}

	function newConversation(){
		root.conversationGeneration++

		if (root.conversationId !== ""){
			root.sendRequest("AssistantCloseConversation", {"conversationId": root.conversationId})
		}

		root.conversationId = ""
		root.messagesModel.clear()
		root.previewStates = {}
		root.clearContextItems()
		root.busy = false
		root.busyCommand = ""
	}

	function sendMessage(text){
		let trimmed = text.trim()
		if (trimmed === "" || root.busy){
			return false
		}

		if (root.conversationId === ""){
			root.__pendingText = trimmed
			root.startConversation()

			return true
		}

		let items = []
		for (let i = 0; i < root.contextItems.length; i++){
			let item = root.contextItems[i]
			items.push({"kind": item.kind, "label": item.label, "dataJson": JSON.stringify(item.data)})
		}

		root.busy = true
		root.busyCommand = "AssistantSendMessage"
		root.sendRequest("AssistantSendMessage", {"conversationId": root.conversationId, "text": trimmed, "contextItems": items})
		root.clearContextItems()

		return true
	}

	/**
		Reload the messages of the conversation from the server, e.g. after a lost answer.
	*/
	function refreshHistory(){
		if (root.conversationId === ""){
			return
		}

		root.sendRequest("AssistantGetHistory", {"conversationId": root.conversationId})
	}

	function cancel(){
		if (!root.busy || root.conversationId === ""){
			return
		}

		root.sendRequest("AssistantCancelRequest", {"conversationId": root.conversationId})
	}

	function confirmPreview(previewId){
		if (root.getPreviewState(previewId) !== ""){
			return
		}

		root.setPreviewState(previewId, "confirming")
		root.sendRequest("AssistantConfirmPreview", {"conversationId": root.conversationId, "previewId": previewId})
	}

	function rejectPreview(previewId){
		if (root.getPreviewState(previewId) !== ""){
			return
		}

		root.setPreviewState(previewId, "rejecting")
		root.sendRequest("AssistantRejectPreview", {"conversationId": root.conversationId, "previewId": previewId})
	}

	function getPreviewState(previewId){
		let state = root.previewStates[previewId]

		return state === undefined ? "" : state
	}

	function setPreviewState(previewId, state){
		let states = Object.assign({}, root.previewStates)
		if (state === ""){
			delete states[previewId]
		}
		else{
			states[previewId] = state
		}

		root.previewStates = states
	}

	// Context released by the user. One item per kind; a new item replaces the old one.
	function addContextItem(kind, label, data){
		let items = []
		for (let i = 0; i < root.contextItems.length; i++){
			if (root.contextItems[i].kind !== kind){
				items.push(root.contextItems[i])
			}
		}

		items.push({"kind": kind, "label": label, "data": data})
		root.contextItems = items
	}

	function removeContextItem(index){
		let items = root.contextItems.slice()
		items.splice(index, 1)
		root.contextItems = items
	}

	function clearContextItems(){
		root.contextItems = []
	}

	function isNonEmptyString(value){
		return typeof value === "string" && value !== ""
	}

	function isOptionalString(object, key){
		return object[key] === undefined || typeof object[key] === "string"
	}

	function isValidRepresentation(representation){
		return representation !== null &&
				typeof representation === "object" &&
				!Array.isArray(representation) &&
				typeof representation.type === "string" &&
				representation.type !== "" &&
				typeof representation.version === "number" &&
				representation.version > 0 &&
				representation.version % 1 === 0 &&
				representation.data !== null &&
				typeof representation.data === "object" &&
				!Array.isArray(representation.data)
	}

	function isValidMessage(message){
		if (message === null || typeof message !== "object"){
			return false
		}

		if (message.formatVersion !== root.formatVersion){
			return false
		}

		if (root.knownKinds.indexOf(message.kind) < 0){
			return false
		}

		if (message.kind === "userText" || message.kind === "assistantText"){
			return typeof message.text === "string"
		}

		if (message.kind === "clarification"){
			if (typeof message.text !== "string" ||
					(message.options !== undefined && !Array.isArray(message.options))){
				return false
			}

			if (message.options !== undefined){
				for (let i = 0; i < message.options.length; i++){
					if (typeof message.options[i] !== "string"){
						return false
					}
				}
			}

			return true
		}

		if (message.kind === "actionPreview"){
			if (!root.isNonEmptyString(message.previewId) ||
					!root.isNonEmptyString(message.actionId) ||
					typeof message.text !== "string" ||
					message.data === null ||
					typeof message.data !== "object" ||
					Array.isArray(message.data) ||
					!Array.isArray(message.data.changes) ||
					typeof message.data.targetLabel !== "string" ||
					typeof message.data.impact !== "string"){
				return false
			}

			for (let i = 0; i < message.data.changes.length; i++){
				let change = message.data.changes[i]
				if (change === null || typeof change !== "object" || Array.isArray(change) ||
						typeof change.field !== "string"){
					return false
				}
			}

			return true
		}

		if (message.kind === "actionResult"){
			return root.isNonEmptyString(message.actionId) &&
					root.isOptionalString(message, "text") &&
					(message.representation === undefined || root.isValidRepresentation(message.representation))
		}

		if (message.kind === "actionExecuted"){
			return root.isNonEmptyString(message.actionId) &&
					root.isNonEmptyString(message.previewId) &&
					root.isNonEmptyString(message.code) &&
					root.isOptionalString(message, "text") &&
					(message.representation === undefined || root.isValidRepresentation(message.representation))
		}

		if (message.kind === "status"){
			return root.isNonEmptyString(message.previewId) && root.isNonEmptyString(message.code)
		}

		if (message.kind === "error"){
			return root.isNonEmptyString(message.code) && root.isOptionalString(message, "text")
		}

		return false
	}

	function appendMessage(message){
		if (!root.isValidMessage(message)){
			message = {"formatVersion": root.formatVersion, "kind": "error", "code": "InvalidMessage"}
		}

		root.messagesModel.append({"kind": message.kind, "messageJson": JSON.stringify(message)})

		// A server answer for a preview ends the pending decision.
		if (message.previewId !== undefined && message.previewId !== "" && message.kind !== "actionPreview"){
			root.setPreviewState(message.previewId, "decided")
		}
	}

	function appendLocalError(code){
		root.appendMessage({"formatVersion": root.formatVersion, "kind": "error", "code": code})
	}

	// Called by the transport with the answer of a command.
	function handleResponse(commandId, conversationId, messagesJson, requestGeneration){
		if (commandId !== "AssistantStartConversation" &&
				((conversationId !== root.conversationId) ||
					(requestGeneration !== undefined && requestGeneration !== root.conversationGeneration))){
			return
		}

		if (commandId === "AssistantStartConversation" &&
				requestGeneration !== undefined && requestGeneration !== root.conversationGeneration){
			return
		}

		let messages = []
		if (messagesJson !== undefined && messagesJson !== ""){
			try {
				messages = JSON.parse(messagesJson)
			}
			catch (error){
				messages = null
			}
		}

		if (commandId === "AssistantStartConversation"){
			root.conversationId = conversationId
		}

		if (commandId === "AssistantGetHistory" && Array.isArray(messages)){
			root.messagesModel.clear()
		}

		if (!Array.isArray(messages)){
			root.appendLocalError("InvalidResponse")
		}
		else{
			for (let i = 0; i < messages.length; i++){
				root.appendMessage(messages[i])
			}
		}

		if (commandId === "AssistantStartConversation" || commandId === "AssistantSendMessage"){
			root.busy = false
			root.busyCommand = ""
		}

		root.messagesAppended()

		if (commandId === "AssistantStartConversation" && root.conversationId !== "" && root.__pendingText !== ""){
			let text = root.__pendingText
			root.__pendingText = ""
			root.sendMessage(text)
		}
		else{
			root.__pendingText = ""
		}
	}

	// Called by the transport if a command failed without an answer.
	function handleError(commandId, message, conversationId, requestGeneration){
		if (commandId !== "AssistantGetStatus" &&
				((conversationId !== undefined && conversationId !== root.conversationId) ||
					(requestGeneration !== undefined && requestGeneration !== root.conversationGeneration))){
			return
		}

		if (commandId === "AssistantGetStatus"){
			root.handleStatus(false)

			return
		}

		if (commandId === "AssistantCloseConversation"){
			return
		}

		if (commandId === "AssistantCancelRequest"){
			root.appendLocalError("CancelFailed")
		}
		else if (commandId === "AssistantGetHistory"){
			root.appendLocalError("HistoryReloadFailed")
		}
		else if (commandId === "AssistantConfirmPreview"){
			// The outcome is unknown; it is not reported as success or failure.
			root.appendLocalError("ConfirmationStatusUnknown")
		}
		else{
			root.appendLocalError("NetworkError")
		}

		if (commandId === "AssistantRejectPreview"){
			root.appendMessage({"formatVersion": root.formatVersion, "kind": "status", "code": "rejectionStatusUnknown"})
		}

		if (commandId === "AssistantStartConversation" || commandId === "AssistantSendMessage"){
			root.busy = false
			root.busyCommand = ""
			root.__pendingText = ""
		}

		root.messagesAppended()
	}
}
