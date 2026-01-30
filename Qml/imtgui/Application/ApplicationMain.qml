import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0

/**
 * ApplicationMain - Main application component (Composition Root)
 * 
 * Refactored following SOLID principles:
 * - Single Responsibility: Acts as composition root, delegates to managers
 * - Open/Closed: Can be extended with new modes without modification
 * - Dependency Inversion: Depends on manager abstractions
 * 
 * Supports three application modes:
 * 1. Standalone (no server, no auth)
 * 2. Server only (server connection, no auth)
 * 3. Full (server + authorization)
 */
Item {
	id: application;
	
	anchors.fill: parent;
	
	// ============================================================================
	// Public Properties (API compatibility maintained)
	// ============================================================================
	
	property Decorators decorators: decorators_
	property alias localSettings: clientSettingsController.json
	property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;

	property bool serverReady: true; // [deprecated]
	property bool authorizationServerConnected: false
	
	// Delegated to configurator
	property bool useWebSocketSubscription: configurator.useWebSocketSubscription
	
	// Delegated to state manager
	property bool firstModelsIsInit: stateManager.firstModelsInitialized
	
	// Delegated to server connection manager
	property bool serverConnected: serverConnectionMgr.isConnected()
	
	property alias subscriptionManager: subscriptionManager_;
	property alias loadPageByClick: thumbnailDecorator.loadPageByClick;
	property alias canRecoveryPassword: thumbnailDecorator.canRecoveryPassword;
	property alias webSocketPortProvider: webSocketPortProvider
	
	// Delegated to connection state manager
	property alias authConnectionState: connectionStateMgr.authConnectionState
	property alias pageAboutProvider: pageAboutProvider
	
	// ============================================================================
	// Managers (SOLID: Single Responsibility for each)
	// ============================================================================
	
	// Application mode configurator
	ApplicationConfigurator {
		id: configurator
		
		Component.onCompleted: {
			// Auto-detect mode based on available features
			autoDetectMode(
				typeof subscriptionManager_ !== 'undefined',
				typeof AuthorizationController !== 'undefined'
			)
		}
		
		onModeChanged: {
			console.log("Application mode changed to:", mode)
			configureManagers({
				connectionStateManager: connectionStateMgr,
				serverConnectionManager: serverConnectionMgr,
				authorizationManager: authMgr
			})
		}
	}
	
	// Connection state management
	ConnectionStateManager {
		id: connectionStateMgr
		
		useWebSocketSubscription: configurator.useWebSocketSubscription
		webSocketStatus: subscriptionManager_.status
		
		onConnectionStateChanged: {
			stateManager.handleConnectionStateChange(status)
		}
	}
	
	// Server connection management
	ServerConnectionManager {
		id: serverConnectionMgr
		
		subscriptionManager: subscriptionManager_
		webSocketPortProvider: webSocketPortProvider
		applicationInfoProvider: applicationInfoProvider
		useWebSocketSubscription: configurator.useWebSocketSubscription
		appId: context.appId
		
		onServerUrlChanged: {
			connectToWebSocket()
		}
	}
	
	// Authorization management
	AuthorizationManager {
		id: authMgr
		
		authorizationController: configurator.hasAuthorization ? AuthorizationController : null
		
		onAuthorizationRequired: {
			thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
		}
		
		onSuperuserCreationRequired: {
			thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
		}
		
		onAuthorizationSuccessful: {
			thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator
			thumbnailDecorator.showPage(undefined)
			stateManager.updateAllModels()
		}
		
		onAuthorizationFailed: {
			stateManager.showMessagePage(qsTr("Authorization failed"))
		}
		
		onLoggedOut: {
			thumbnailDecorator.stopLoading()
			NavigationController.clear()
			subscriptionManager_.clear()
		}
	}
	
	// Application state management
	ApplicationStateManager {
		id: stateManager
		
		thumbnailDecorator: thumbnailDecorator
		connectionStateManager: connectionStateMgr
		
		onShowErrorPage: {
			delayedErrorTimer.errorMessage = message
			delayedErrorTimer.loadingVisible = loading
			delayedErrorTimer.restart()
		}
		
		onClearErrorPage: {
			delayedErrorTimer.stop()
		}
		
		onModelsInitialized: {
			application.firstModelsInit()
		}
	}
	
	// Timer for delayed error display
	Timer {
		id: delayedErrorTimer
		interval: 1000
		repeat: false
		
		property string errorMessage: ""
		property bool loadingVisible: false
		
		onTriggered: {
			thumbnailDecorator.stackView.clear()
			Events.sendEvent("SearchVisible", false)
			Events.sendEvent("SetUserPanelEnabled", false)
			application.showMessagePage(errorMessage, loadingVisible)
		}
	}
	
	// ============================================================================
	// Legacy Code (TODO: Move to managers)
	// ============================================================================
	
	// TODO: ConnectionStateMachine.qml [DEPRECATED - replaced by ConnectionStateManager]
	QtObject {
		id: connectionState

		property int totalStatus: connectionStateMgr.totalStatus
		
		property var status : connectionStateMgr.ConnectionStatus
		
		// 0 - WebSocket.Connecting, 1 - WebSocket.Open, 2 - WebSocket.Closing, 3 - WebSocket.Closed, 4 - WebSocket.Error
		property int appConnectionState: connectionStateMgr.appConnectionState
		property int authConnectionState: connectionStateMgr.authConnectionState

		onAppConnectionStateChanged: {
			// Handled by ConnectionStateManager
		}
		
		onAuthConnectionStateChanged: {
			// Handled by ConnectionStateManager
		}
		
		function checkState(){
			// Handled by ConnectionStateManager
		}
		
		onTotalStatusChanged: {
			// Handled by ApplicationStateManager
		}
	}
	
	Timer {
		id: statusTimer
		interval: 1000
		repeat: false
		onTriggered: {
			// Handled by ApplicationStateManager via delayedErrorTimer
		}
	}
	
	QtObject {
		id: internal
		property string errorMessage: "";
		property bool loadingVisible: false;
	}

	// ============================================================================
	// Public Methods (API compatibility maintained)
	// ============================================================================
	
	function showMessagePage(message, loadingVisible){
		if (!loadingVisible){
			loadingVisible = false
		}

		internal.errorMessage = message;
		internal.loadingVisible = loadingVisible;
		
		thumbnailDecorator.stackView.addPage(messagePageComp);
	}
	
	function setDecorators(){
		Style.setDecorators(decorators)
	}
	
	function getServerUrl(){
		return serverConnectionMgr.getServerUrl()
	}
	
	function reconnect(){
		serverConnectionMgr.reconnect()
	}
	
	function getHeaders(){
		return serverConnectionMgr.getHeaders(AuthorizationController.productId)
	}
	
	function updateAllModels(){
		userSettingsController.getSettings()
		stateManager.updateAllModels()
	}
	
	function onSimpleUserManagement(){
		application.updateAllModels()
	}
	
	function onStrongUserManagement(){
		if (configurator.hasAuthorization) {
			let loggedUserId = AuthorizationController.getLoggedUserId()
			if (loggedUserId === ""){
				AuthorizationController.updateSuperuserModel()
			}
		}
	}
	
	function firstModelsInit(force){
		if (!force){
			force = false
		}
		
		if (!force && stateManager.firstModelsInitialized){
			return
		}
		
		if (configurator.hasAuthorization) {
			authMgr.initialize()
		}
		
		stateManager.initializeModels(force)
	}
	
	function connectToWebSocketServer(){
		serverConnectionMgr.connectToWebSocket()
	}
	
	// ============================================================================
	// Signals
	// ============================================================================
	
	signal saveSettings(string json);
	signal settingsSaved();
	signal settingsSaveFailed();
	
	// ============================================================================
	// Initialization
	// ============================================================================
	
	Component.onCompleted: {
		setDecorators()
		timer.onTriggeredFunc()
	}
	
	onWidthChanged: {
		Events.sendEvent("AppSizeChanged", {"Width": width, "Height": height})
		Events.sendEvent("AppWidthChanged", width)
	}
	
	onHeightChanged: {
		Events.sendEvent("AppSizeChanged", {"Width": width, "Height": height})
		Events.sendEvent("AppHeightChanged", height)
	}
	
	onServerConnectedChanged: {
		if (serverConnected){
			serverConnectionMgr.updateApplicationInfo()
		}
	}
	
	// ============================================================================
	// Visual Components
	// ============================================================================
	
	Decorators {
		id: decorators_
	}
	
	DecoratorsQt {
		id: decoratorsQt;
	}
	
	property alias thumbnailDecoratorGui: thumbnailDecorator;
	
	// ============================================================================
	// Providers and Controllers
	// ============================================================================
	
	property ApplicationInfoProvider applicationInfoProvider : ApplicationInfoProvider
	{
		onUpdated: {
			if (serverApplicationInfo){
				context.appId = serverApplicationInfo.m_applicationId;
				context.appName = serverApplicationInfo.m_applicationName;
				
				pageAboutProvider.serverVersion = serverApplicationInfo.m_version
				
				if (configurator.hasAuthorization) {
					AuthorizationController.productId = serverApplicationInfo.m_applicationId
				}
			}
		}
	}
	
	property DesignSchemaProvider designProvider : DesignSchemaProvider
	{
	}
	
	property LanguageProvider languageProvider : LanguageProvider
	{
	}
	
	ClientSettingsController {
		id: clientSettingsController
		
		onSaveSettings: {
			application.saveSettings(paramsSet.toJson())
		}
		
		onUrlChanged: {
			serverConnectionMgr.setServerUrl(getServerUrl())
			serverConnectionMgr.reconnect()
		}
		
		Component.onCompleted: {
			serverConnectionMgr.setServerUrl(getServerUrl())
		}
	}
	
	UserSettingsController {
		id: userSettingsController
		
		onParamModelCreated: {
			let languageSelectionParam = application.languageProvider.selectionParam;
			let languageParamter = getParameterById(application.languageProvider.typeId);
			if (languageParamter.m_data !== languageSelectionParam.toJson()){
				application.languageProvider.selectionParam.createFromJson(languageParamter.m_data)
			}
			
			let designSelectionParam = application.designProvider.selectionParam;
			let designParamter = getParameterById(application.designProvider.typeId)
			if (designParamter.m_data !== designSelectionParam.toJson()){
				application.designProvider.selectionParam.createFromJson(designParamter.m_data)
			}
		}
	}
	
	PatTokenSettingsController {
		id: patTokenSettingsController
	}
	
	PageAboutProvider {
		id: pageAboutProvider
	}
	
	SubscriptionManager {
		id: subscriptionManager_;
		active: configurator.useWebSocketSubscription;
	}
	
	property Component messagePageComp: Component {
		ServerNoConnectionView {
			z: 5;
			anchors.fill: parent;
			anchors.topMargin: thumbnailDecorator.topPanel.height;
			loadingVisible: internal.loadingVisible;
			text: internal.errorMessage;
		}
	}
	
	WebSocketPortProvider {
		id: webSocketPortProvider;
		
		onPortChanged: {
			serverConnectionMgr.connectToWebSocket()
		}
	}
	
	Component {
		id: serverLogProviderComp
		ServerLogProvider {}
	}
	
	ThumbnailDecorator {
		id: thumbnailDecorator;
		
		anchors.fill: parent;
		Component.onCompleted: {
			settingsController.registerParamEditor("ServerLog", serverLogProviderComp)
		}
		
		function fillPreferenceParamsSet(){
			if (configurator.showNetworkSettings){
				settingsController.registerParamsSetController("Network", qsTr("Network"), clientSettingsController)
			}

			if (application.serverConnected){
				settingsController.registerParamsSetController("General", qsTr("General"), userSettingsController)
				settingsController.registerParamsSetController("About", qsTr("About"), pageAboutProvider)

				// Register PAT tokens settings (always available)
				settingsController.registerParamsSetController("PatTokens", qsTr("PAT Tokens"), patTokenSettingsController)
			}
		}
	}
	
	// ============================================================================
	// Authorization Connections
	// ============================================================================
	
	Connections {
		target: configurator.hasAuthorization ? AuthorizationController : null
		enabled: configurator.hasAuthorization
		
		function onUserModeChanged(userMode){
			authMgr.handleUserModeChange(userMode)
		}
		
		function onSuperuserExistResult(status, message){
			authMgr.handleSuperuserExistResult(status, message)
		}
		
		function onLoggedIn(){
			authMgr.handleLoggedIn()
		}
		
		function onLoggedOut(){
			authMgr.handleLoggedOut()
		}
	}
	
	// ============================================================================
	// WebSocket Port Update Timer
	// ============================================================================
	
	property Timer timer: Timer{
		interval: 3000;
		repeat: true;
		running: configurator.useWebSocketSubscription && webSocketPortProvider.port == -1;
		onTriggered: {
			onTriggeredFunc();
		}
		
		function onTriggeredFunc(){
			if (!configurator.useWebSocketSubscription){
				return;
			}
			
			if (webSocketPortProvider.port == -1){
				webSocketPortProvider.updateModel();
			}
		}
	}
}


