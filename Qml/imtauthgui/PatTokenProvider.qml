import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtauthPersonalAccessTokensSdl 1.0


/**
	Provider for Personal Access Token operations.
	
	This component abstracts the protocol-specific implementation (GraphQL)
	from the UI layer. The editor uses only the public interface (signals and functions)
	without knowing about the underlying transport protocol.
	
	Benefits:
	- Protocol independence: can switch from GraphQL to REST/gRPC without changing the editor
	- Testability: can mock the provider for testing
	- Single responsibility: protocol logic separated from UI logic
	- Reusability: can be used by multiple components
*/
QtObject {
	id: root
	
	// Signals for communicating results back to consumers
	signal tokenListReceived(var tokenList)
	signal tokenCreated(bool success, string message, string token)
	signal tokenDeleted(bool success, string message)
	signal tokenRevoked(bool success, string message)
	
	// Input objects for GraphQL requests
	property CreateTokenInput createTokenInput: CreateTokenInput {}
	property InputId deleteInputId: InputId {}
	property InputId revokeInputId: InputId {}
	property UserIdInput userIdInput: UserIdInput {}
	
	/**
		Get list of tokens for the current user.
		
		Result is returned via tokenListReceived signal.
		
		\param userId The user ID to get tokens for
	*/
	function getTokenList(userId) {
		userIdInput.m_userId = userId
		getTokenListRequest.send(userIdInput)
	}
	
	/**
		Create a new token.
		
		Result is returned via tokenCreated signal.
		
		\param userId User ID who owns the token
		\param name Token name
		\param description Token description
		\param scopes List of permission scopes
		\param expiresAt Expiration date in ISO format (empty string for no expiration)
	*/
	function createToken(userId, name, description, scopes, expiresAt) {
		createTokenInput.m_userId = userId
		createTokenInput.m_name = name
		createTokenInput.m_description = description
		createTokenInput.m_scopes = scopes
		createTokenInput.m_expiresAt = expiresAt
		
		createTokenRequest.send(createTokenInput)
	}
	
	/**
		Delete a token permanently.
		
		Result is returned via tokenDeleted signal.
		
		\param tokenId Token ID to delete
	*/
	function deleteToken(tokenId) {
		deleteInputId.m_id = tokenId
		deleteTokenRequest.send(deleteInputId)
	}
	
	/**
		Revoke a token (mark as invalid but keep for audit).
		
		Result is returned via tokenRevoked signal.
		
		\param tokenId Token ID to revoke
	*/
	function revokeToken(tokenId) {
		revokeInputId.m_id = tokenId
		revokeTokenRequest.send(revokeInputId)
	}
	
	// Internal GraphQL request handlers
	// These can be replaced with different protocol implementations
	
	property GqlSdlRequestSender getTokenListRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_getTokenList
		sdlObjectComp: Component {
			PersonalAccessTokenList {
				onFinished: {
					root.tokenListReceived(this)
				}
			}
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
	}
}
