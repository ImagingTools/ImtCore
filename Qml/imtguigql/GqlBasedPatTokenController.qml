import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtauthPersonalAccessTokensSdl 1.0


/**
	GraphQL-based implementation of PatTokenController.
	
	This controller encapsulates all GraphQL-specific logic for managing
	personal access tokens. It inherits from PatTokenController and implements
	its functions using GqlSdlRequestSender components.
*/
PatTokenController {
	id: root
	
	// HTTP headers
	function getHeaders(){
		return {}
	}
	
	// Input objects for GraphQL requests
	property CreateTokenInput createTokenInput: CreateTokenInput {}
	property InputId deleteInputId: InputId {}
	property InputId revokeInputId: InputId {}
	property UserIdInput userIdInput: UserIdInput {}
	
	// Override: Get token list
	function getTokenList(userId) {
		userIdInput.m_userId = userId
		getTokenListRequest.send(userIdInput)
	}
	
	// Override: Create token
	function createToken(userId, name, description, scopes, expiresAt) {
		createTokenInput.m_userId = userId
		createTokenInput.m_name = name
		createTokenInput.m_description = description
		createTokenInput.m_scopes = scopes
		createTokenInput.m_expiresAt = expiresAt
		
		createTokenRequest.send(createTokenInput)
	}
	
	// Override: Delete token
	function deleteToken(tokenId) {
		deleteInputId.m_id = tokenId
		deleteTokenRequest.send(deleteInputId)
	}
	
	// Override: Revoke token
	function revokeToken(tokenId) {
		revokeInputId.m_id = tokenId
		revokeTokenRequest.send(revokeInputId)
	}
	
	// GraphQL request handlers
	
	property GqlSdlRequestSender getTokenListRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_getTokenList
		sdlObjectComp: Component {
			PersonalAccessTokenList {
				onFinished: {
					root.tokenListReceived(this)
				}
			}
		}
		
		function getHeaders(){
			return root.getHeaders()
		}
	}
	
	property GqlSdlRequestSender createTokenRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_createToken
		requestType: 1 // Mutation
		sdlObjectComp: Component {
			CreateTokenPayload {
				onFinished: {
					root.tokenCreated(m_success, m_message, m_token)
				}
			}
		}
		
		function getHeaders(){
			return root.getHeaders()
		}
	}
	
	property GqlSdlRequestSender deleteTokenRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_deleteToken
		requestType: 1 // Mutation
		sdlObjectComp: Component {
			DeleteTokenPayload {
				onFinished: {
					root.tokenDeleted(m_success, m_message)
				}
			}
		}
		
		function getHeaders(){
			return root.getHeaders()
		}
	}
	
	property GqlSdlRequestSender revokeTokenRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_revokeToken
		requestType: 1 // Mutation
		sdlObjectComp: Component {
			RevokeTokenPayload {
				onFinished: {
					root.tokenRevoked(m_success, m_message)
				}
			}
		}
		
		function getHeaders(){
			return root.getHeaders()
		}
	}
}
