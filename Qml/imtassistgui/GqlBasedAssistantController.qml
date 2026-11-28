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

	Component {
		id: requestSenderComponent

		GqlRequestSender {
			id: requestSender

			property string commandId: ""
			property int requestGeneration: 0
			property string requestConversationId: ""

			gqlCommandId: requestSender.commandId
			requestType: commandId === "AssistantGetStatus" || commandId === "AssistantGetHistory" ? 0 : 1
			permissionPath: root.permissionPath

			function createQueryParams(query, params){
				query.AddParam(root.createInput(params))

				if (requestSender.commandId === "AssistantGetStatus"){
					query.AddField(Gql.GqlObject("available"))
				}
				else if (requestSender.commandId === "AssistantCancelRequest"){
					query.AddField(Gql.GqlObject("canceled"))
				}
				else if (requestSender.commandId === "AssistantCloseConversation"){
					query.AddField(Gql.GqlObject("closed"))
				}
				else{
					root.addMessageFields(query)
				}
			}

			function onResult(data){
				if (requestSender.commandId === "AssistantGetStatus"){
					root.handleStatus(data && data.containsKey("available") && data.getData("available") === true)
				}
				else if (requestSender.commandId !== "AssistantCancelRequest" && requestSender.commandId !== "AssistantCloseConversation"){
					root.handleResponse(
								requestSender.commandId,
								root.readString(data, "conversationId"),
								root.readString(data, "messagesJson"),
								requestSender.requestGeneration)
				}

				requestSender.destroy()
			}

			function onError(message, type){
				root.handleError(
							requestSender.commandId,
							message,
							requestSender.requestConversationId,
							requestSender.requestGeneration)
				requestSender.destroy()
			}
		}
	}

	// reimplemented (AssistantController)
	function sendRequest(commandId, input){
		if (["AssistantGetStatus", "AssistantStartConversation", "AssistantSendMessage", "AssistantConfirmPreview",
				"AssistantRejectPreview", "AssistantCancelRequest", "AssistantGetHistory", "AssistantCloseConversation"].indexOf(commandId) < 0){
			console.warn("GqlBasedAssistantController: unsupported command", commandId)

			return
		}

		let requestSender = requestSenderComponent.createObject(root)
		requestSender.commandId = commandId
		requestSender.requestConversationId = input.conversationId === undefined ? "" : input.conversationId
		requestSender.requestGeneration = root.conversationGeneration
		requestSender.send(input)
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

}
