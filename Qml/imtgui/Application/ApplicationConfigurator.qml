import QtQuick 2.15

/**
 * ApplicationConfigurator - Configures application based on deployment mode
 * 
 * Responsibilities:
 * - Define application mode (standalone, server, auth)
 * - Configure managers based on mode
 * - Provide feature flags
 * 
 * Following Open/Closed Principle - easy to extend with new modes
 * Following Dependency Inversion - depends on abstractions (managers)
 */
QtObject {
	id: root
	
	// Application modes
	readonly property var ApplicationMode: ({
		STANDALONE: "standalone",        // No server, no auth
		SERVER_ONLY: "server_only",     // Server connection, no auth
		FULL: "full"                    // Server + Authorization
	})
	
	// Current mode (can be set from outside)
	property string mode: ApplicationMode.STANDALONE
	
	// Feature flags based on mode
	readonly property bool hasServer: mode === ApplicationMode.SERVER_ONLY || mode === ApplicationMode.FULL
	readonly property bool hasAuthorization: mode === ApplicationMode.FULL
	readonly property bool requiresConnection: hasServer
	
	// Configuration properties
	readonly property bool useWebSocketSubscription: hasServer
	readonly property bool showNetworkSettings: hasServer && Qt.platform.os !== "web"
	readonly property bool showAuthorizationUI: hasAuthorization
	
	// Signals
	signal modeChanged(string newMode)
	
	// Public methods
	
	/**
	 * Set application mode
	 */
	function setMode(newMode) {
		if (isValidMode(newMode) && mode !== newMode) {
			mode = newMode
			modeChanged(newMode)
		}
	}
	
	/**
	 * Auto-detect mode based on configuration
	 */
	function autoDetectMode(hasServerConnection, hasAuthController) {
		if (hasAuthController) {
			setMode(ApplicationMode.FULL)
		}
		else if (hasServerConnection) {
			setMode(ApplicationMode.SERVER_ONLY)
		}
		else {
			setMode(ApplicationMode.STANDALONE)
		}
	}
	
	/**
	 * Check if mode is valid
	 */
	function isValidMode(checkMode) {
		return checkMode === ApplicationMode.STANDALONE ||
		       checkMode === ApplicationMode.SERVER_ONLY ||
		       checkMode === ApplicationMode.FULL
	}
	
	/**
	 * Get configuration summary
	 */
	function getConfigurationSummary() {
		return {
			mode: mode,
			hasServer: hasServer,
			hasAuthorization: hasAuthorization,
			requiresConnection: requiresConnection,
			useWebSocketSubscription: useWebSocketSubscription
		}
	}
	
	/**
	 * Configure managers based on current mode
	 */
	function configureManagers(managers) {
		if (!managers) {
			console.error("ApplicationConfigurator: Invalid managers object")
			return
		}
		
		// Configure ConnectionStateManager
		if (managers.connectionStateManager) {
			managers.connectionStateManager.useWebSocketSubscription = useWebSocketSubscription
		}
		
		// Configure ServerConnectionManager
		if (managers.serverConnectionManager) {
			managers.serverConnectionManager.useWebSocketSubscription = useWebSocketSubscription
		}
		
		// Enable/disable authorization checks
		if (managers.authorizationManager) {
			// Authorization manager is only used when hasAuthorization is true
			// This is handled at the ApplicationMain level
		}
		
		console.log("ApplicationConfigurator: Configured for mode:", mode)
	}
}
