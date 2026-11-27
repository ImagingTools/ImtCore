// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtassistgui 1.0

/**
	GqlBasedAssistantController

	AssistantController that talks to imtassistgql::CAssistantControllerComp over GraphQL.
	Every command has its own sender, so that a cancel or a confirmation can be sent while
	a message is still being processed.
*/
AssistantController {
	id: root

	property string permissionPath: ""

	// reimplemented (AssistantController)
	function sendRequest(commandId, input){
		if (commandId === "AssistantStartConversation"){
			startSender.send(input)
		}
		else if (commandId === "AssistantSendMessage"){
			sendMessageSender.send(input)
		}
		else if (commandId === "AssistantConfirmPreview"){
			confirmSender.send(input)
		}
		else if (commandId === "AssistantRejectPreview"){
			rejectSender.send(input)
		}
		else if (commandId === "AssistantCancelRequest"){
			cancelSender.send(input)
		}
		else if (commandId === "AssistantGetHistory"){
			historySender.send(input)
		}
		else if (commandId === "AssistantCloseConversation"){
			closeSender.send(input)
		}
		else{
			console.warn("GqlBasedAssistantController: unsupported command", commandId)
		}
	}

	function createInput(params){
		let input = Gql.GqlObject("input")
		if (params.conversationId !== undefined){
			input.InsertField("conversationId", params.conversationId)
		}

		if (params.previewId !== undefined){
			input.InsertField("previewId", params.previewId)
		}

		if (params.text !== undefined){
			input.InsertField("text", params.text)
		}

		if (params.contextItems !== undefined){
			let items = []
			for (let i = 0; i < params.contextItems.length; i++){
				let item = Gql.GqlObject("")
				item.InsertField("kind", params.contextItems[i].kind)
				item.InsertField("label", params.contextItems[i].label)
				item.InsertField("dataJson", params.contextItems[i].dataJson)
				items.push(item)
			}

			input.InsertField("contextItems", items)
		}

		return input
	}

	function addMessageFields(query){
		query.AddField(Gql.GqlObject("conversationId"))
		query.AddField(Gql.GqlObject("messagesJson"))
	}

	function readString(data, key){
		if (data && data.containsKey(key)){
			return data.getData(key)
		}

		return ""
	}

	property GqlRequestSender startSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantStartConversation"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			root.addMessageFields(query)
		}

		function onResult(data){
			root.handleResponse("AssistantStartConversation", root.readString(data, "conversationId"), root.readString(data, "messagesJson"))
		}

		function onError(message, type){
			root.handleError("AssistantStartConversation", message)
		}
	}

	property GqlRequestSender sendMessageSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantSendMessage"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			root.addMessageFields(query)
		}

		function onResult(data){
			root.handleResponse("AssistantSendMessage", root.conversationId, root.readString(data, "messagesJson"))
		}

		function onError(message, type){
			root.handleError("AssistantSendMessage", message)
		}
	}

	property GqlRequestSender confirmSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantConfirmPreview"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			root.addMessageFields(query)
		}

		function onResult(data){
			root.handleResponse("AssistantConfirmPreview", root.conversationId, root.readString(data, "messagesJson"))
		}

		function onError(message, type){
			root.handleError("AssistantConfirmPreview", message)
		}
	}

	property GqlRequestSender rejectSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantRejectPreview"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			root.addMessageFields(query)
		}

		function onResult(data){
			root.handleResponse("AssistantRejectPreview", root.conversationId, root.readString(data, "messagesJson"))
		}

		function onError(message, type){
			root.handleError("AssistantRejectPreview", message)
		}
	}

	property GqlRequestSender cancelSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantCancelRequest"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			query.AddField(Gql.GqlObject("canceled"))
		}

		function onResult(data){
		}

		function onError(message, type){
			root.handleError("AssistantCancelRequest", message)
		}
	}

	property GqlRequestSender historySender: GqlRequestSender {
		requestType: 0
		gqlCommandId: "AssistantGetHistory"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			root.addMessageFields(query)
		}

		function onResult(data){
			root.handleResponse("AssistantGetHistory", root.conversationId, root.readString(data, "messagesJson"))
		}

		function onError(message, type){
			root.handleError("AssistantGetHistory", message)
		}
	}

	property GqlRequestSender closeSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantCloseConversation"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			query.AddField(Gql.GqlObject("closed"))
		}

		function onResult(data){
		}

		function onError(message, type){
			root.handleError("AssistantCloseConversation", message)
		}
	}
}
