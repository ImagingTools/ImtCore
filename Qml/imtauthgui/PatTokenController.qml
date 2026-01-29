import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0


/**
	Base controller for Personal Access Token operations.
	
	This is a protocol-agnostic interface that defines the operations
	and signals for managing personal access tokens. Concrete implementations
	(e.g., GqlBasedPatTokenController) override the functions to provide
	protocol-specific implementations.
*/
QtObject {
	id: root
	
	// Signals for communicating results back to consumers
	signal tokenListReceived(var tokenList)
	signal tokenCreated(bool success, string message, string token)
	signal tokenDeleted(bool success, string message)
	signal tokenRevoked(bool success, string message)
	
	/**
		Get list of tokens for a specific user.
		
		Result is returned via tokenListReceived signal.
		
		\param userId The user ID to get tokens for
	*/
	function getTokenList(userId) {
		console.warn("getTokenList() should be implemented in a subclass")
		tokenListReceived(null)
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
		console.warn("createToken() should be implemented in a subclass")
		tokenCreated(false, "Not implemented", "")
	}
	
	/**
		Delete a token permanently.
		
		Result is returned via tokenDeleted signal.
		
		\param tokenId Token ID to delete
	*/
	function deleteToken(tokenId) {
		console.warn("deleteToken() should be implemented in a subclass")
		tokenDeleted(false, "Not implemented")
	}
	
	/**
		Revoke a token (mark as invalid but keep for audit).
		
		Result is returned via tokenRevoked signal.
		
		\param tokenId Token ID to revoke
	*/
	function revokeToken(tokenId) {
		console.warn("revokeToken() should be implemented in a subclass")
		tokenRevoked(false, "Not implemented")
	}
}
