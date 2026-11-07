// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtbaseImtCollectionSdl 1.0
import imtauthServerTokensSdl 1.0
import imtauthgui 1.0
import imtguigql 1.0

/**
 * The tokens of product servers over GraphQL.
 */
ServerTokenApiClient {
	id: root

	function requestTokens(productId){
		root.__listInput.m_clientProductId = productId
		root.__listRequest.send(root.__listInput)
	}

	function createToken(productId, receiverId, name, description, scopes){
		root.__productId = productId
		root.__createInput.m_clientProductId = productId
		root.__createInput.m_productId = receiverId
		root.__createInput.m_name = name
		root.__createInput.m_description = description
		root.__createInput.m_scopes = scopes
		root.__createRequest.send(root.__createInput)
	}

	function revokeToken(productId, tokenId){
		root.__productId = productId
		root.__revokeInput.m_id = tokenId
		root.__revokeRequest.send(root.__revokeInput)
	}

	function requestProductServer(productId){
		root.__receiversInput.m_clientProductId = productId
		root.__receiversRequest.send(root.__receiversInput)
	}

	function updateTokenScopes(productId, tokenId, scopes){
		root.__productId = productId
		root.__scopesInput.m_id = tokenId
		root.__scopesInput.m_scopes = scopes
		root.__scopesRequest.send(root.__scopesInput)
	}

	// The server answers with itself only, or with nothing when it gives the product no token
	function __parseServer(serversList){
		let server = serversList && serversList.count > 0 ? serversList.get(0).item : null
		if (!server){
			return null
		}

		return {
			"productId": String(server.m_productId),
			"groups": root.__parseGroups(server.m_groups),
			"requiredPermissions": Array.isArray(server.m_requiredPermissions) ? server.m_requiredPermissions : []
		}
	}

	function __parseGroups(groupsList){
		let result = []
		if (!groupsList){
			return result
		}

		for (let gi = 0; gi < groupsList.count; ++gi){
			let group = groupsList.get(gi).item
			if (!group){
				continue
			}

			let groupObj = {
				"groupId": group.m_groupId ? String(group.m_groupId) : "",
				"groupName": group.m_groupName ? String(group.m_groupName) : "",
				"entries": []
			}

			let entries = group.m_entries
			if (entries){
				for (let ei = 0; ei < entries.count; ++ei){
					let entry = entries.get(ei).item
					if (!entry){
						continue
					}

					groupObj.entries.push({
						"permissionId": entry.m_permissionId ? String(entry.m_permissionId) : "",
						"displayName": entry.m_displayName ? String(entry.m_displayName) : "",
						"description": entry.m_description ? String(entry.m_description) : "",
						"requirements": Array.isArray(entry.m_requirements) ? entry.m_requirements : []
					})
				}
			}

			result.push(groupObj)
		}

		return result
	}

	property string __productId: ""
	property ServerTokenListInput __listInput: ServerTokenListInput {}
	property CreateServerTokenInput __createInput: CreateServerTokenInput {}
	property InputId __revokeInput: InputId {}
	property ServerTokenReceiversInput __receiversInput: ServerTokenReceiversInput {}
	property UpdateServerTokenScopesInput __scopesInput: UpdateServerTokenScopesInput {}

	property GqlSdlRequestSender __receiversRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_getServerTokenReceivers
		sdlObjectComp: Component {
			ServerTokenReceiverList {
				onFinished: {
					root.productServer = root.__parseServer(m_productServers)
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}

	property GqlSdlRequestSender __scopesRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_updateServerTokenScopes
		requestType: 1
		sdlObjectComp: Component {
			RevokeServerTokenPayload {
				onFinished: {
					if (!m_success){
						root.operationFailed(m_message)
					}

					root.requestTokens(root.__productId)
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}

	property GqlSdlRequestSender __listRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_getServerTokenList
		sdlObjectComp: Component {
			ServerTokenList {
				onFinished: {
					root.tokens = m_tokens
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}

	property GqlSdlRequestSender __createRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_createServerToken
		requestType: 1
		sdlObjectComp: Component {
			CreateServerTokenPayload {
				onFinished: {
					if (m_success){
						root.tokenCreated(m_token)
						root.requestTokens(root.__productId)
					}
					else{
						root.operationFailed(m_message)
					}
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}

	property GqlSdlRequestSender __revokeRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_revokeServerToken
		requestType: 1
		sdlObjectComp: Component {
			RevokeServerTokenPayload {
				onFinished: {
					if (!m_success){
						root.operationFailed(m_message)
					}

					root.requestTokens(root.__productId)
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}
}
