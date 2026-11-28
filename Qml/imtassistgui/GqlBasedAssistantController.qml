// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtassistgui 1.0

/**
	GqlBasedAssistantController

	AssistantController that talks to imtassistgql::CAssistantControllerComp over GraphQL.
	Each command has its own sender, so that a cancel or a confirmation can be sent while
	a message is still being processed.
*/
AssistantController {
	id: root

	property string permissionPath: ""

	// reimplemented (AssistantController)
	function sendRequest(commandId, input){
		let requestParams = Object.assign({}, input, {"requestGeneration": root.requestGeneration})
		if (commandId === "AssistantGetStatus"){
			root.statusSender.send(requestParams)
		}
		else if (commandId === "AssistantStartConversation"){
			root.startSender.send(requestParams)
		}
		else if (commandId === "AssistantSendMessage"){
			root.sendMessageSender.send(requestParams)
		}
		else if (commandId === "AssistantConfirmPreview"){
			root.confirmSender.send(requestParams)
		}
		else if (commandId === "AssistantRejectPreview"){
			root.rejectSender.send(requestParams)
		}
		else if (commandId === "AssistantCancelRequest"){
			root.cancelSender.send(requestParams)
		}
		else if (commandId === "AssistantGetHistory"){
			root.historySender.send(requestParams)
		}
		else if (commandId === "AssistantCloseConversation"){
			root.closeSender.send(requestParams)
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

	property GqlRequestSender statusSender: GqlRequestSender {
		requestType: 0
		gqlCommandId: "AssistantGetStatus"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			query.AddField(Gql.GqlObject("available"))
		}

		function onResult(data, params){
			root.handleStatus(data && data.containsKey("available") && data.getData("available") === true, params.requestGeneration)
		}

		function onError(message, type, params){
			root.handleError("AssistantGetStatus", message, params.requestGeneration)
		}
	}

	property GqlRequestSender startSender: GqlRequestSender {
		requestType: 1
		gqlCommandId: "AssistantStartConversation"
		permissionPath: root.permissionPath

		function createQueryParams(query, params){
			query.AddParam(root.createInput(params))
			root.addMessageFields(query)
		}

		function onResult(data, params){
			root.handleResponse("AssistantStartConversation", root.readString(data, "conversationId"), root.readString(data, "messagesJson"), params.requestGeneration)
		}

		function onError(message, type, params){
			root.handleError("AssistantStartConversation", message, params.requestGeneration)
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

		function onResult(data, params){
			root.handleResponse("AssistantSendMessage", root.readString(data, "conversationId"), root.readString(data, "messagesJson"), params.requestGeneration)
		}

		function onError(message, type, params){
			root.handleError("AssistantSendMessage", message, params.requestGeneration)
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

		function onResult(data, params){
			root.handleResponse("AssistantConfirmPreview", root.readString(data, "conversationId"), root.readString(data, "messagesJson"), params.requestGeneration)
		}

		function onError(message, type, params){
			root.handleError("AssistantConfirmPreview", message, params.requestGeneration)
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

		function onResult(data, params){
			root.handleResponse("AssistantRejectPreview", root.readString(data, "conversationId"), root.readString(data, "messagesJson"), params.requestGeneration)
		}

		function onError(message, type, params){
			root.handleError("AssistantRejectPreview", message, params.requestGeneration)
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

		function onError(message, type, params){
			root.handleError("AssistantCancelRequest", message, params.requestGeneration)
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

		function onResult(data, params){
			root.handleResponse("AssistantGetHistory", root.readString(data, "conversationId"), root.readString(data, "messagesJson"), params.requestGeneration)
		}

		function onError(message, type, params){
			root.handleError("AssistantGetHistory", message, params.requestGeneration)
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

		function onError(message, type, params){
			root.handleError("AssistantCloseConversation", message, params.requestGeneration)
		}
	}
}
