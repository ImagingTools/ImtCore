import QtQuick 2.15

/**
 * ServerConnectionManager - Manages server connections and WebSocket subscriptions
 * 
 * Responsibilities:
 * - Manage WebSocket connection lifecycle
 * - Handle server URL configuration
 * - Manage subscription manager
 * - Handle reconnection logic
 * 
 * Following Single Responsibility Principle - only manages server connections
 */
QtObject {
	id: root
	
	// Public properties
	property var subscriptionManager: null
	property var webSocketPortProvider: null
	property var applicationInfoProvider: null
	property bool useWebSocketSubscription: false
	property string appId: ""
	
	// Signals
	signal serverConnected()
	signal serverDisconnected()
	signal serverUrlChanged(string url)
	
	// Private properties
	QtObject {
		id: internal
		property string serverUrl: ""
	}
	
	// Public methods
	
	/**
	 * Get current server URL
	 */
	function getServerUrl() {
		return internal.serverUrl
	}
	
	/**
	 * Set server URL
	 */
	function setServerUrl(url) {
		if (internal.serverUrl !== url) {
			internal.serverUrl = url
			serverUrlChanged(url)
		}
	}
	
	/**
	 * Reconnect to server
	 */
	function reconnect() {
		if (webSocketPortProvider) {
			webSocketPortProvider.port = -1
		}
	}
	
	/**
	 * Connect to WebSocket server
	 */
	function connectToWebSocket() {
		if (!useWebSocketSubscription || !subscriptionManager) {
			return
		}
		
		if (!webSocketPortProvider || webSocketPortProvider.port < 0) {
			console.warn("ServerConnectionManager: Invalid WebSocket port")
			return
		}
		
		subscriptionManager.active = false
		
		let webSocketUrl = getWebSocketUrl(internal.serverUrl)
		if (webSocketUrl === "") {
			console.error("ServerConnectionManager: Failed to generate WebSocket URL")
			return
		}
		
		subscriptionManager.url = webSocketUrl
		subscriptionManager.active = true
	}
	
	/**
	 * Update application info from server
	 */
	function updateApplicationInfo() {
		if (applicationInfoProvider) {
			applicationInfoProvider.updateModel()
		}
	}
	
	/**
	 * Get request headers for server communication
	 */
	function getHeaders(productId) {
		return {"productId": productId || ""}
	}
	
	/**
	 * Check if connected to server
	 */
	function isConnected() {
		if (!useWebSocketSubscription) {
			return true // No WebSocket required, consider as connected
		}
		
		if (!subscriptionManager) {
			return false
		}
		
		return subscriptionManager.status === 1 // WebSocket.Open
	}
	
	// Private methods
	
	/**
	 * Generate WebSocket URL from server URL
	 */
	function getWebSocketUrl(serverUrl) {
		try {
			let url = new URL(serverUrl)
			
			let protocol = "ws"
			if (url.protocol === "https:") {
				protocol = "wss"
			}
			
			url.protocol = protocol
			
			if (webSocketPortProvider && webSocketPortProvider.port >= 0) {
				url.port = webSocketPortProvider.port
			}
			else {
				console.error("ServerConnectionManager: WebSocket port provider has invalid port!")
				return ""
			}
			
			if (appId && appId !== "") {
				url.pathname = "/" + appId + "/wssub"
			}
			
			return String(url)
		}
		catch(error) {
			console.error("ServerConnectionManager: Error generating WebSocket URL:", error)
			return ""
		}
	}
}
