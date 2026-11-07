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

	function requestTokens(){
		root.__listInput.m_clientProductId = root.productId
		root.__listRequest.send(root.__listInput)
	}

	function createToken(name, description, scopes){
		root.__createInput.m_clientProductId = root.productId
		root.__createInput.m_name = name
		root.__createInput.m_description = description
		root.__createInput.m_scopes = scopes
		root.__createRequest.send(root.__createInput)
	}

	function revokeToken(tokenId){
		root.__revokeInput.m_id = tokenId
		root.__revokeRequest.send(root.__revokeInput)
	}

	function requestProductServer(){
		root.__receiverInput.m_clientProductId = root.productId
		root.__receiverRequest.send(root.__receiverInput)
	}

	function updateTokenScopes(tokenId, scopes){
		root.__scopesInput.m_id = tokenId
		root.__scopesInput.m_scopes = scopes
		root.__scopesRequest.send(root.__scopesInput)
	}

	function __parseServer(server){
		return {
			"productId": String(server.m_productId),
			"groups": PermissionGroups.toFlatArray(server.m_groups),
			"requiredPermissions": Array.isArray(server.m_requiredPermissions) ? server.m_requiredPermissions : []
		}
	}

	property ServerTokenListInput __listInput: ServerTokenListInput {}
	property CreateServerTokenInput __createInput: CreateServerTokenInput {}
	property InputId __revokeInput: InputId {}
	property ServerTokenReceiverInput __receiverInput: ServerTokenReceiverInput {}
	property UpdateServerTokenScopesInput __scopesInput: UpdateServerTokenScopesInput {}

	property GqlSdlRequestSender __receiverRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_getServerTokenReceiver
		sdlObjectComp: Component {
			ServerTokenReceiver {
				onFinished: {
					root.productServer = root.__parseServer(this)
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
			UpdateServerTokenScopesPayload {
				onFinished: {
					if (!m_success){
						root.operationFailed(m_message)
					}

					root.requestTokens()
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
						root.requestTokens()
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

					root.requestTokens()
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}
}
