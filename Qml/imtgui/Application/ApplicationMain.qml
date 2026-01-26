import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0


Item {
	id: application;
	
	anchors.fill: parent;
	
	property Decorators decorators: decorators_
	
	property alias localSettings: clientSettingsController.json
	
	property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;

	property bool serverReady: true; // [deprecated]
	property bool authorizationServerConnected: false;
	property bool useWebSocketSubscription: false;
	
	property bool firstModelsIsInit: false;
	property bool serverConnected: subscriptionManager_.status == 1;
	
	property alias subscriptionManager: subscriptionManager_;
	
	property alias loadPageByClick: thumbnailDecorator.loadPageByClick;
	property alias canRecoveryPassword: thumbnailDecorator.canRecoveryPassword;
	property alias webSocketPortProvider: webSocketPortProvider

	property alias authConnectionState: connectionState.authConnectionState
	property alias pageAboutProvider: pageAboutProvider
	
	// TODO: ConnectionStateMachine.qml
	QtObject {
		id: connectionState

		property int totalStatus: -1
		
		property var status : ({
			TRY_CONNECTING_TO_AUTH_SERVER: 0,
			AUTH_SERVER_CONNECTION_ERROR: 1,
			TRY_CONNECTING_TO_APP_SERVER: 2,
			APP_SERVER_CONNECTION_ERROR: 3,
			CONNECTION_SUCCESFUL: 4
		})
		
		// 0 - WebSocket.Connecting, 1 - WebSocket.Open, 2 - WebSocket.Closing, 3 - WebSocket.Closed, 4 - WebSocket.Error
		property int appConnectionState: application.useWebSocketSubscription ? subscriptionManager_.status : 1
		property int authConnectionState: 1 // -1 - Unknown, 0 - Connecting, 1 - Connected, 2 - Disconnected

		onAppConnectionStateChanged: {
			checkState()
		}
		
		onAuthConnectionStateChanged: {
			checkState()
		}
		
		function checkState(){
			if (authConnectionState == 1 && appConnectionState == 1){
				totalStatus = status.CONNECTION_SUCCESFUL
			}
			else if (appConnectionState == 0){
				totalStatus = status.TRY_CONNECTING_TO_APP_SERVER
			}
			else if (appConnectionState == 4 || appConnectionState == 3){
				totalStatus = status.APP_SERVER_CONNECTION_ERROR
			}
			else if (authConnectionState == 0){
				totalStatus = status.TRY_CONNECTING_TO_AUTH_SERVER
			}
			else if (authConnectionState == 2){
				totalStatus = status.AUTH_SERVER_CONNECTION_ERROR
			}
		}
		
		onTotalStatusChanged: {
			if (totalStatus == status.TRY_CONNECTING_TO_AUTH_SERVER){
				internal.errorMessage = qsTr("Try connecting to authorization server ...")
				internal.loadingVisible = true
			}
			else if (totalStatus == status.AUTH_SERVER_CONNECTION_ERROR){
				internal.errorMessage = qsTr("Authorization server connection error")
				internal.loadingVisible = false
			}
			else if (totalStatus == status.TRY_CONNECTING_TO_APP_SERVER){
				internal.errorMessage = qsTr("Try connecting to ") + application.getServerUrl() + " ..."
				internal.loadingVisible = true
			}
			else if (totalStatus == status.APP_SERVER_CONNECTION_ERROR){
				internal.errorMessage = qsTr("Server connection error")
				internal.loadingVisible = false
			}
			else if (totalStatus == status.CONNECTION_SUCCESFUL){
				thumbnailDecorator.stackView.clear();
				statusTimer.stop()
				Events.sendEvent("SearchVisible", true);
				Events.sendEvent("SetUserPanelEnabled", true);
				
				application.firstModelsInit()
				
				return
			}

			statusTimer.restart()
		}
	}
	
	Timer {
		id: statusTimer
		interval: 1000
		repeat: false
		onTriggered: {
			thumbnailDecorator.stackView.clear();
			
			Events.sendEvent("SearchVisible", false);
			Events.sendEvent("SetUserPanelEnabled", false);
			
			application.showMessagePage(internal.errorMessage, internal.loadingVisible)
		}
	}
	
	QtObject {
		id: internal
		property string errorMessage: "";
		property bool loadingVisible: false;
	}

	function showMessagePage(message, loadingVisible){
		if (!loadingVisible){
			loadingVisible = false
		}

		internal.errorMessage = message;
		internal.loadingVisible = loadingVisible;
		
		thumbnailDecorator.stackView.addPage(messagePageComp);
	}
	
	signal saveSettings(string json);
	signal settingsSaved();
	signal settingsSaveFailed();
	
	Component.onCompleted: {
		setDecorators()
		timer.onTriggeredFunc();
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
			applicationInfoProvider.updateModel();
		}
	}
	
	Decorators {
		id: decorators_
	}
	
	DecoratorsQt {
		id: decoratorsQt;
	}
	
	function setDecorators(){
		Style.setDecorators(decorators)
	}
	
	function getServerUrl(){
		return clientSettingsController.getServerUrl()
	}
	
	function reconnect(){
		webSocketPortProvider.port = -1;
	}
	
	function getHeaders(){
		return {"productId": AuthorizationController.productId};
	}
	
	property alias thumbnailDecoratorGui: thumbnailDecorator;
	
	property ApplicationInfoProvider applicationInfoProvider : ApplicationInfoProvider
	{
		onUpdated: {
			if (serverApplicationInfo){
				context.appId = serverApplicationInfo.m_applicationId;
				context.appName = serverApplicationInfo.m_applicationName;
				
				pageAboutProvider.serverVersion = serverApplicationInfo.m_version
				
				AuthorizationController.productId = serverApplicationInfo.m_applicationId
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
			// AuthorizationController.logout();
			application.reconnect();
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
		
		Component.onCompleted: {
			// Initialize PAT tokens parameter
			let patTokensParam = paramsSetController.getParameterById("PatTokens")
			if (!patTokensParam || !patTokensParam.m_id) {
				paramsSetController.addParam("PatTokenList", "PatTokens", qsTr("PAT Tokens"), qsTr("Personal Access Tokens for API authentication"), "{\"tokens\":[]}")
			}
		}
	}
	
	PageAboutProvider {
		id: pageAboutProvider
	}
	
	SubscriptionManager {
		id: subscriptionManager_;
		active: application.useWebSocketSubscription;
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
			application.connectToWebSocketServer();
		}
	}
	
	function getWebSocketUrl(serverUrl){
		try {
			let url = new URL(serverUrl);

			let protocol = "ws";
			if (url.protocol === "https:"){
				protocol = "wss";
			}

			url.protocol = protocol

			if (webSocketPortProvider.port >= 0){
				url.port = webSocketPortProvider.port;
			}
			else{
				console.error("WebSocket port provider has invalid port!");
			}

			if (context.appId && context.appId !== ""){
				url.pathname = "/" + context.appId + "/wssub";
			}

			return String(url)
		}
		catch(error){
			console.error(error);
			return "";
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
			if (Qt.platform.os !== "web"){
				settingsController.registerParamsSetController("Network", qsTr("Network"), clientSettingsController)
			}
			
			// Register PAT tokens settings (always available)
			settingsController.registerParamsSetController("PatTokens", qsTr("PAT Tokens"), patTokenSettingsController)
			
			if (application.serverConnected){
				settingsController.registerParamsSetController("General", qsTr("General"), userSettingsController)
				settingsController.registerParamsSetController("About", qsTr("About"), pageAboutProvider)
			}
		}
	}
	
	function updateAllModels(){
		userSettingsController.getSettings()
		thumbnailDecorator.updateModels();
	}
	
	function onSimpleUserManagement(){
		application.updateAllModels();
	}
	
	function onStrongUserManagement(){
		let loggedUserId = AuthorizationController.getLoggedUserId();
		if (loggedUserId === ""){
			AuthorizationController.updateSuperuserModel();
		}
	}
	
	function firstModelsInit(force){
		if (!force){
			force = false
		}
		
		if (!force && firstModelsIsInit){
			return
		}
		
		let loggedUserId = AuthorizationController.getLoggedUserId();
		if (loggedUserId === ""){
			AuthorizationController.updateUserManagementModel();
		}
		
		firstModelsIsInit = true;
	}
	
	function connectToWebSocketServer(){
		if (!application.useWebSocketSubscription){
			return;
		}
		
		subscriptionManager_.active = false;
		let serverUrl = getServerUrl();
		let webSocketServerUrl = getWebSocketUrl(serverUrl);
		subscriptionManager_.url = webSocketServerUrl;
		subscriptionManager_.active = true;
	}
	
	Connections {
		target: AuthorizationController;
		
		function onUserModeChanged(userMode){
			if (AuthorizationController.isStrongUserManagement()){
				application.onStrongUserManagement();
			}
			else if (AuthorizationController.isSimpleUserManagement()){
				application.onSimpleUserManagement();
			}
		}
		
		function onSuperuserExistResult(status, message){
			if (status === "EXISTS"){
				thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
			}
			else if (status === "NOT_EXISTS"){
				thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
			}
			else{
				// UNKNOWN
				application.showMessagePage(message);
			}
		}
		
		function onLoggedIn(){
			thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator;
			thumbnailDecorator.showPage(undefined)
			
			application.updateAllModels();
		}
		
		function onLoggedOut(){
			thumbnailDecorator.stopLoading();
			application.firstModelsInit(true);
			NavigationController.clear();
			subscriptionManager_.clear();
		}
	}
	
	property Timer timer: Timer{
		interval: 3000;
		repeat: true;
		running: application.useWebSocketSubscription && webSocketPortProvider.port == -1;
		onTriggered: {
			onTriggeredFunc();
		}
		
		function onTriggeredFunc(){
			if (!application.useWebSocketSubscription){
				return;
			}
			
			if (webSocketPortProvider.port == -1){
				webSocketPortProvider.updateModel();
			}
		}
	}
}


