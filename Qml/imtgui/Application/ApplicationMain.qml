import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import QtWebSockets 1.2


Item {
    id: application;

    anchors.fill: parent;

    property Decorators decorators: decorators_

    property alias localSettings: application.settingsProvider.localModel;

    property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;

    property string applicationId;
    property var applicationInfo;

    property bool serverReady: false;
    property bool authorizationServerConnected: false;
    property bool useWebSocketSubscription: false;

    property alias subscriptionManager: subscriptionManager_;

    property alias loadPageByClick: thumbnailDecorator.loadPageByClick;
    property alias canRecoveryPassword: thumbnailDecorator.canRecoveryPassword;

    property bool start: serverReady && application.settingsProvider.localModel != null && webSocketPortProvider.port != -1;
    onStartChanged: {
        if (start){
            connectToWebSocketServer();
        }
    }

    onAuthorizationServerConnectedChanged: {
        console.log("onAuthorizationServerConnectedChanged", authorizationServerConnected);
        if (authorizationServerConnected){
            let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();
            if (loggedUserId === ""){
                thumbnailDecorator.closeAllPages();

                application.firstModelsInit();
            }
        }
    }

    onWidthChanged: {
        Events.sendEvent("AppSizeChanged", {"Width": width, "Height": height})
    }

    onHeightChanged: {
        Events.sendEvent("AppSizeChanged", {"Width": width, "Height": height})
    }

    onApplicationInfoChanged: {
        console.log("onApplicationInfoChanged");
        if (application.applicationInfo){
            applicationInfoProvider.clientApplicationInfo = application.applicationInfo;
        }
    }

    onServerReadyChanged: {
        if (serverReady){
            webSocketPortProvider.updateModel();
        }
    }

    signal updateSystemStatus();
    signal settingsUpdate();
    signal localSettingsUpdated();

    onLocalSettingsUpdated: {
        application.updateAllModels();
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
        if (application.settingsProvider){
            let serverUrl = application.settingsProvider.getValue("ServerUrl");

            return serverUrl;
        }

        return "";
    }

    Component.onCompleted: {
        setDecorators()

        Events.subscribeEvent("UpdateModels", application.updateAllModels);
        Events.subscribeEvent("Logout", application.onLogout);

        thumbnailDecorator.userManagementProvider.updated.connect(application.onUserModeChanged);

        thumbnailDecorator.loadingPage.start();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateModels", application.updateAllModels);
        Events.unSubscribeEvent("Logout", application.onLogout);
    }

    property alias thumbnailDecoratorGui: thumbnailDecorator;

    property ApplicationInfoProvider applicationInfoProvider : ApplicationInfoProvider
    {
    }

    property SettingsObserver settingsObserver : SettingsObserver
    {
        designProvider: application.designProvider;
        languageProvider: application.languageProvider;
        settingsProvider: application.settingsProvider;
    }

    property SettingsProvider settingsProvider : SettingsProvider
    {
        applicationInfoProvider: application.applicationInfoProvider;

        onServerModelChanged: {
            console.log("onServerModelChanged", serverModel);
            let design = application.designProvider.getDesignSchema();

            console.log("design", design);

            let index = application.designProvider.getDesignSchemaIndex(design);
            if (index >= 0){
                application.settingsObserver.onDesignSchemaChanged(index);
            }
        }

        onLocalSettingsSaved: {
            application.settingsUpdate();
        }

        property bool applyCachedLanguage: application.serverReady && application.settingsProvider.serverModel != null;
        onApplyCachedLanguageChanged: {
            console.log("onApplyCachedLanguageChanged", applyCachedLanguage);
            if (applyCachedLanguage){
                let lang = application.languageProvider.getLanguage();
                application.languageProvider.setLanguage(lang);
            }
        }

        property bool applyCachedSchema: application.serverReady && application.settingsProvider.localModel != null;
        onApplyCachedSchemaChanged: {
            console.log("onApplyCachedSchemaChanged", applyCachedSchema);
            if (applyCachedSchema){
                application.designProvider.applyCachedDesignSchema();
            }
        }
    }

    property DesignSchemaProvider designProvider : DesignSchemaProvider
    {
        settingsProvider: application.settingsProvider;
    }

    property LanguageProvider languageProvider : LanguageProvider
    {
        settingsProvider: application.settingsProvider;
    }

    SubscriptionManager {
        id: subscriptionManager_;

        active: application.useWebSocketSubscription;

        onStatusChanged: {
            application.checkStatus(status)
        }
    }

    function checkStatus(status){
        thumbnailDecorator.closeAllPages();
        thumbnailDecorator.messagePage.loadingVisible = false;

        if (status === WebSocket.Connecting){
            thumbnailDecorator.messagePage.visible = true;
            thumbnailDecorator.messagePage.loadingVisible = true;

            let serverUrl = application.getServerUrl();
            thumbnailDecorator.messagePage.text = qsTr("Try connecting to ") + serverUrl + " ...";
        }
        else if (status === WebSocket.Error || status === WebSocket.Closed){
            thumbnailDecorator.messagePage.visible = true;
            thumbnailDecorator.messagePage.text = qsTr("Server connection error")
        }
        else if (status === WebSocket.Open){
            // No error
            let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();
            if (loggedUserId === ""){
                thumbnailDecorator.closeAllPages();

                application.firstModelsInit();
            }
        }
        else if (status === 5){
            thumbnailDecorator.messagePage.visible = true;
            thumbnailDecorator.messagePage.text = qsTr("Authorization server connection error")
        }
    }

    WebSocketPortProvider {
        id: webSocketPortProvider;

        onPortChanged: {
            application.connectToWebSocketServer();
        }
    }

    function getWebSocketUrl(serverUrl){
        console.log("getWebSocketUrl", serverUrl);

        try {
            let url = new URL(serverUrl);

            url.protocol = "ws";

            if (context.appName && context.appName !== ""){
                url.pathname = "/" + context.appName + "/wssub";
            }

            if (webSocketPortProvider.port >= 0){
                url.port = webSocketPortProvider.port;
            }
            else{
                console.error("WebSocket port provider has invalid port!");
            }

            return String(url)
        }
        catch(error){
            return "";
        }
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;

        anchors.fill: parent;

        settingsProvider: application.settingsProvider;
        settingsObserver: application.settingsObserver;

        applicationMain: application;
    }

    function onLogout(){
        settingsProvider.serverModel = null;

        subscriptionManager_.clear();
    }

    function updateAllModels(){
        thumbnailDecorator.updateModels();
        applicationInfoProvider.updateModel();
    }

    function onSimpleUserManagement(){
        let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();
        if (loggedUserId === ""){
            thumbnailDecorator.authorizationPageAlias.setLoggedUserId("Anonim");

            application.updateAllModels();
        }
    }

    function onStrongUserManagement(){
        let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();
        if (loggedUserId === ""){
            if (!authorizationServerConnected){
                checkStatus(5);
                return;
            }

            thumbnailDecorator.superuserProvider.superuserExists();
        }
    }

    function firstModelsInit(){
        console.log("firstModelsInit");
        thumbnailDecorator.userManagementProvider.updateModel();
    }

    function onUserModeChanged(){
        let userMode = thumbnailDecorator.userManagementProvider.userMode;
        let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();

        if (userMode == "NO_USER_MANAGEMENT" || userMode == "OPTIONAL_USER_MANAGEMENT"){
            application.onSimpleUserManagement();
        }
        else if (userMode == "STRONG_USER_MANAGEMENT"){
            application.onStrongUserManagement();
        }
    }

    function connectToWebSocketServer(){
        subscriptionManager_.active = false;
        let serverUrl = getServerUrl();
        let webSocketServerUrl = getWebSocketUrl(serverUrl);
        subscriptionManager_.url = webSocketServerUrl;
        subscriptionManager_.active = true;
    }

    property Timer timer: Timer{
        id: timer;

        interval: 3000;

        repeat: true;
        running: true;

        onTriggered: {
            if (webSocketPortProvider.port == -1){
                application.checkStatus(WebSocket.Error)

                if (application.serverReady){
                    webSocketPortProvider.updateModel();
                }
            }
        }
    }
}


