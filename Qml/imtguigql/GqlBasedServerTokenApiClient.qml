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

	property string __productId: ""
	property ServerTokenListInput __listInput: ServerTokenListInput {}
	property CreateServerTokenInput __createInput: CreateServerTokenInput {}
	property InputId __revokeInput: InputId {}

	property GqlSdlRequestSender __listRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthServerTokensSdlCommandIds.s_getServerTokenList
		sdlObjectComp: Component {
			ServerTokenList {
				onFinished: {
					root.tokens = m_tokens
					root.tokensReceived()
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
					root.requestTokens(root.__productId)
				}
			}
		}

		function onError(message, type){
			root.operationFailed(message)
		}
	}
}
