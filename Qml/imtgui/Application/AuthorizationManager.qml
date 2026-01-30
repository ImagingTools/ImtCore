import QtQuick 2.15

/**
 * AuthorizationManager - Manages authorization-related logic
 * 
 * Responsibilities:
 * - Handle user management mode changes
 * - Manage superuser existence checks
 * - Handle login/logout events
 * - Update authorization models
 * 
 * Following Single Responsibility Principle - only manages authorization logic
 */
QtObject {
	id: root
	
	// Public properties
	property var authorizationController: null
	
	// Signals
	signal authorizationRequired()
	signal superuserCreationRequired()
	signal authorizationSuccessful()
	signal authorizationFailed()
	signal loggedOut()
	
	// Public methods
	
	/**
	 * Initialize authorization - check if superuser exists
	 */
	function initialize() {
		if (!authorizationController) {
			console.error("AuthorizationManager: AuthorizationController not set")
			return
		}
		
		let loggedUserId = authorizationController.getLoggedUserId()
		if (loggedUserId === "") {
			authorizationController.updateUserManagementModel()
		}
	}
	
	/**
	 * Handle user management mode changes
	 */
	function handleUserModeChange(userMode) {
		if (authorizationController.isStrongUserManagement()) {
			onStrongUserManagement()
		}
		else if (authorizationController.isSimpleUserManagement()) {
			onSimpleUserManagement()
		}
	}
	
	/**
	 * Handle superuser existence check result
	 */
	function handleSuperuserExistResult(status, message) {
		if (status === "EXISTS") {
			authorizationRequired()
		}
		else if (status === "NOT_EXISTS") {
			superuserCreationRequired()
		}
		else {
			// UNKNOWN
			authorizationFailed()
		}
	}
	
	/**
	 * Handle successful login
	 */
	function handleLoggedIn() {
		authorizationSuccessful()
	}
	
	/**
	 * Handle logout
	 */
	function handleLoggedOut() {
		initialize()
		loggedOut()
	}
	
	/**
	 * Check if user is logged in
	 */
	function isUserLoggedIn() {
		if (!authorizationController) {
			return false
		}
		return authorizationController.getLoggedUserId() !== ""
	}
	
	// Private methods
	
	function onSimpleUserManagement() {
		// For simple user management, no additional checks needed
		authorizationSuccessful()
	}
	
	function onStrongUserManagement() {
		if (!authorizationController) {
			return
		}
		
		let loggedUserId = authorizationController.getLoggedUserId()
		if (loggedUserId === "") {
			authorizationController.updateSuperuserModel()
		}
	}
}
