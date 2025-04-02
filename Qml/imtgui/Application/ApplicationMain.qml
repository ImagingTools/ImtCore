import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0

Item {
	id: application;
	
	anchors.fill: parent;
	
	property Decorators decorators: decorators_
	
	property alias localSettings: clientSettingsController.json
	
	property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;
	
	property string applicationId;
	property var applicationInfo;
	
	property bool serverReady: true;
	property bool authorizationServerConnected: false;
	property bool useWebSocketSubscription: false;
	
	property bool firstModelsIsInit: true;
	property bool serverConnected: subscriptionManager_.status == 1;
	
	property alias subscriptionManager: subscriptionManager_;
	
	property alias loadPageByClick: thumbnailDecorator.loadPageByClick;
	property alias canRecoveryPassword: thumbnailDecorator.canRecoveryPassword;
	
	onAuthorizationServerConnectedChanged: {
		application.firstModelsInit();
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
		return {"ProductId": context.appId};
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
			let languageParamter = getParamJsonByPath([application.languageProvider.typeId]);
			if (languageParamter != languageSelectionParam.toJson()){
				application.languageProvider.selectionParam.createFromJson(languageParamter)
			}
			
			let designSelectionParam = application.designProvider.selectionParam;
			let designParamter = getParamJsonByPath([application.designProvider.typeId]);
			if (designParamter != designSelectionParam.toJson()){
				application.designProvider.selectionParam.createFromJson(designParamter)
			}
		}
	}
	
	PageAboutProvider {
		id: pageAboutProvider
	}
	
	SubscriptionManager {
		id: subscriptionManager_;
		
		active: application.useWebSocketSubscription;
		
		onStatusChanged: {
			application.checkStatus(status)
			
			if (status == 1){
				application.firstModelsInit();
			}
		}
	}
	
	function checkStatus(status){
		thumbnailDecorator.stackView.clear();
		
		//	0 - WebSocket.Connecting
		//	1 - WebSocket.Open
		//	2 - WebSocket.Closing
		//	3 - WebSocket.Closed
		//	4 - WebSocket.Error
		//	5 - Authorization Error
		
		Events.sendEvent("SearchVisible", false);
		Events.sendEvent("SetUserPanelEnabled", false);
		
		if (status === 0){
			showMessagePage(qsTr("Try connecting to ") + application.getServerUrl() + " ...");
		}
		else if (status === 4 || status === 3){
			showMessagePage(qsTr("Server connection error"));
		}
		else if (status === 1){
			Events.sendEvent("SearchVisible", true);
			Events.sendEvent("SetUserPanelEnabled", true);
		}
		else if (status === 5){
			showMessagePage(qsTr("Authorization server connection error"));
		}
	}
	
	function showMessagePage(message){
		application.errorMessage = message;
		
		thumbnailDecorator.stackView.push(messagePageComp);
	}
	
	property Component messagePageComp: Component {
		ServerNoConnectionView {
			z: 5;
			anchors.fill: parent;
			anchors.topMargin: thumbnailDecorator.topPanel.height;
			loadingVisible: false;
			text: application.errorMessage;
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
			if (Qt.platform.os != "web"){
				settingsController.registerParamsSetController("Network", qsTr("Network"), clientSettingsController)
			}
			
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
			if (!authorizationServerConnected){
				checkStatus(5);
				return;
			}
			
			AuthorizationController.updateSuperuserModel();
		}
	}
	
	function firstModelsInit(){
		if (AuthorizationController.isStrongUserManagement() && !authorizationServerConnected){
			checkStatus(5);
			
			return;
		}
		
		checkStatus(1);
		
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
	
	property string errorMessage: "";
	
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
		
		function onSuperuserExistResult(exists, type, message){
			if (exists){
				thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
			}
			else{
				if (type === 'NotExists'){
					thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
				}
				else if (type === 'DbNotConnection'){
					application.showMessagePage(message);
				}
			}
		}
		
		function onLoggedIn(){
			thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator;
			thumbnailDecorator.showPage(undefined)
			
			application.updateAllModels();
		}
		
		function onLoggedOut(){
			thumbnailDecorator.stopLoading();
			application.firstModelsInit();
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
				application.checkStatus(4)
				
				webSocketPortProvider.updateModel();
			}
		}
	}
}


