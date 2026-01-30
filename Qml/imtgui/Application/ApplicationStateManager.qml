import QtQuick 2.15

/**
 * ApplicationStateManager - Manages application UI state
 * 
 * Responsibilities:
 * - Track application initialization state
 * - Manage error messages and loading states
 * - Control UI visibility based on connection state
 * - Handle page navigation for connection errors
 * 
 * Following Single Responsibility Principle - only manages UI state
 */
QtObject {
	id: root
	
	// Public properties
	property bool firstModelsInitialized: false
	property string currentErrorMessage: ""
	property bool loadingVisible: false
	
	// Dependencies
	property var thumbnailDecorator: null
	property var connectionStateManager: null
	
	// Signals
	signal showErrorPage(string message, bool loading)
	signal clearErrorPage()
	signal modelsInitialized()
	
	// Public methods
	
	/**
	 * Show message page with error
	 */
	function showMessagePage(message, loading) {
		currentErrorMessage = message
		loadingVisible = loading !== undefined ? loading : false
		showErrorPage(message, loadingVisible)
	}
	
	/**
	 * Initialize models for the first time
	 */
	function initializeModels(force) {
		if (!force && firstModelsInitialized) {
			return
		}
		
		firstModelsInitialized = true
		modelsInitialized()
	}
	
	/**
	 * Reset initialization state (e.g., after logout)
	 */
	function resetInitialization() {
		firstModelsInitialized = false
	}
	
	/**
	 * Handle connection state changes
	 */
	function handleConnectionStateChange(connectionStatus) {
		if (!connectionStateManager) {
			return
		}
		
		let status = connectionStateManager.ConnectionStatus
		
		if (connectionStatus === status.CONNECTION_SUCCESFUL) {
			handleSuccessfulConnection()
		}
		else {
			handleConnectionError(connectionStatus)
		}
	}
	
	/**
	 * Update UI for successful connection
	 */
	function handleSuccessfulConnection() {
		if (thumbnailDecorator) {
			thumbnailDecorator.stackView.clear()
		}
		
		Events.sendEvent("SearchVisible", true)
		Events.sendEvent("SetUserPanelEnabled", true)
		
		clearErrorPage()
		modelsInitialized()
	}
	
	/**
	 * Update UI for connection errors
	 */
	function handleConnectionError(status) {
		if (!connectionStateManager) {
			return
		}
		
		let message = connectionStateManager.getStatusMessage()
		let loading = connectionStateManager.isLoadingState()
		
		Events.sendEvent("SearchVisible", false)
		Events.sendEvent("SetUserPanelEnabled", false)
		
		showMessagePage(message, loading)
	}
	
	/**
	 * Update all application models
	 */
	function updateAllModels() {
		if (thumbnailDecorator) {
			thumbnailDecorator.updateModels()
		}
	}
	
	/**
	 * Clear all models (e.g., on logout)
	 */
	function clearModels() {
		resetInitialization()
		
		if (thumbnailDecorator) {
			thumbnailDecorator.stackView.clear()
		}
	}
}
