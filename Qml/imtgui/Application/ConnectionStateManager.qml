import QtQuick 2.15

/**
 * ConnectionStateManager - Manages connection states for WebSocket and Authorization servers
 * 
 * Responsibilities:
 * - Track WebSocket connection state
 * - Track Authorization server connection state
 * - Calculate overall connection status
 * - Emit state change notifications
 * 
 * Following Single Responsibility Principle - only manages connection states
 */
QtObject {
	id: root
	
	// Public properties
	property bool useWebSocketSubscription: false
	property int webSocketStatus: 1 // 0 - Connecting, 1 - Open, 2 - Closing, 3 - Closed, 4 - Error
	property int authConnectionState: 1 // -1 - Unknown, 0 - Connecting, 1 - Connected, 2 - Disconnected
	
	// Read-only status
	readonly property int totalStatus: internal.totalStatus
	readonly property bool isConnected: totalStatus === ConnectionStatus.CONNECTION_SUCCESFUL
	
	// Enum-like object for connection statuses
	readonly property var ConnectionStatus: ({
		TRY_CONNECTING_TO_AUTH_SERVER: 0,
		AUTH_SERVER_CONNECTION_ERROR: 1,
		TRY_CONNECTING_TO_APP_SERVER: 2,
		APP_SERVER_CONNECTION_ERROR: 3,
		CONNECTION_SUCCESFUL: 4
	})
	
	// Signals
	signal connectionStateChanged(int status)
	
	// Internal state
	QtObject {
		id: internal
		property int totalStatus: -1
	}
	
	// Computed property for app connection state
	readonly property int appConnectionState: useWebSocketSubscription ? webSocketStatus : 1
	
	// Watchers
	onAppConnectionStateChanged: checkState()
	onAuthConnectionStateChanged: checkState()
	
	// Public methods
	function checkState() {
		let newStatus = -1
		
		if (authConnectionState === 1 && appConnectionState === 1) {
			newStatus = ConnectionStatus.CONNECTION_SUCCESFUL
		}
		else if (appConnectionState === 0) {
			newStatus = ConnectionStatus.TRY_CONNECTING_TO_APP_SERVER
		}
		else if (appConnectionState === 4 || appConnectionState === 3) {
			newStatus = ConnectionStatus.APP_SERVER_CONNECTION_ERROR
		}
		else if (authConnectionState === 0) {
			newStatus = ConnectionStatus.TRY_CONNECTING_TO_AUTH_SERVER
		}
		else if (authConnectionState === 2) {
			newStatus = ConnectionStatus.AUTH_SERVER_CONNECTION_ERROR
		}
		
		if (internal.totalStatus !== newStatus) {
			internal.totalStatus = newStatus
			connectionStateChanged(newStatus)
		}
	}
	
	function getStatusMessage() {
		switch (totalStatus) {
			case ConnectionStatus.TRY_CONNECTING_TO_AUTH_SERVER:
				return qsTr("Try connecting to authorization server ...")
			case ConnectionStatus.AUTH_SERVER_CONNECTION_ERROR:
				return qsTr("Authorization server connection error")
			case ConnectionStatus.TRY_CONNECTING_TO_APP_SERVER:
				return qsTr("Try connecting to app server ...")
			case ConnectionStatus.APP_SERVER_CONNECTION_ERROR:
				return qsTr("Server connection error")
			case ConnectionStatus.CONNECTION_SUCCESFUL:
				return qsTr("Connected successfully")
			default:
				return qsTr("Unknown connection state")
		}
	}
	
	function isLoadingState() {
		return totalStatus === ConnectionStatus.TRY_CONNECTING_TO_AUTH_SERVER ||
		       totalStatus === ConnectionStatus.TRY_CONNECTING_TO_APP_SERVER
	}
}
