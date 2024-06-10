import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0


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

    property bool updatingModel: serverReady && application.settingsProvider.serverModel != null;

    onAuthorizationServerConnectedChanged: {
        application.firstModelsInit();
    }

    signal updateSystemStatus();
    signal settingsUpdate();
    signal localSettingsUpdated();

    onUpdatingModelChanged: {
        if (updatingModel){
            Events.sendEvent("UpdateModels");
        }
    }

    onWidthChanged: {
        Events.sendEvent("AppSizeChanged", {"Width": width, "Height": height})
    }

    onHeightChanged: {
        Events.sendEvent("AppSizeChanged", {"Width": width, "Height": height})
    }

    onApplicationInfoChanged: {
        if (application.applicationInfo){
            applicationInfoProvider.clientApplicationInfo = application.applicationInfo;
        }
    }

    onServerReadyChanged: {
        if (serverReady){
            webSocketPortProvider.updateModel();
        }
    }

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

    function reconnect(){
        webSocketPortProvider.port = -1;
    }

    Component.onCompleted: {
        setDecorators()

        Events.subscribeEvent("UpdateModels", application.updateAllModels);
        Events.subscribeEvent("Logout", application.onLogout);
        Events.subscribeEvent("Reconnect", application.reconnect);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateModels", application.updateAllModels);
        Events.unSubscribeEvent("Logout", application.onLogout);
        Events.unSubscribeEvent("Reconnect", application.reconnect);
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
            let design = application.designProvider.getDesignSchema();

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
            if (applyCachedLanguage){
                let lang = application.languageProvider.getLanguage();
                application.languageProvider.setLanguage(lang);

                application.settingsUpdate();
            }
        }

        property bool applyCachedSchema: application.serverReady && application.settingsProvider.localModel != null;
        onApplyCachedSchemaChanged: {
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

            if (status == 1){
                application.firstModelsInit();
            }
        }
    }

    function checkStatus(status){
        thumbnailDecorator.stackView.clear();

//        0 - WebSocket.Connecting
//        1 - WebSocket.Open
//        2 - WebSocket.Closing
//        3 - WebSocket.Closed
//        4 - WebSocket.Error
//        5 - Authorization Error

        Events.sendEvent("SetCommandsVisible", false);
        Events.sendEvent("SetUserPanelEnabled", false);

        if (status === 0){
            thumbnailDecorator.stackView.push(connectingMessagePageComp);
        }
        else if (status === 4 || status === 3){
            thumbnailDecorator.stackView.push(errorMessagePageComp);
        }
        else if (status === 1){
            Events.sendEvent("SetCommandsVisible", true);
            Events.sendEvent("SetUserPanelEnabled", true);
        }
        else if (status === 5){
            thumbnailDecorator.stackView.push(authorizatioErrorMessagePageComp);
        }
    }

    property Component connectingMessagePageComp: Component {
        ServerNoConnectionView {
            z: 5;
            anchors.fill: parent;
            anchors.topMargin: thumbnailDecorator.topPanel.height;
            loadingVisible: true;
            text: qsTr("Try connecting to ") + application.getServerUrl() + " ...";
        }
    }

    property Component errorMessagePageComp: Component {
        ServerNoConnectionView {
            z: 5;
            anchors.fill: parent;
            anchors.topMargin: thumbnailDecorator.topPanel.height;
            loadingVisible: false;
            text: qsTr("Server connection error")
        }
    }

    property Component authorizatioErrorMessagePageComp: Component {
        ServerNoConnectionView {
            z: 5;
            anchors.fill: parent;
            anchors.topMargin: thumbnailDecorator.topPanel.height;
            loadingVisible: false;
            text: qsTr("Authorization server connection error")
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

            if (url.protocol === "https:"){
                url.protocol = "wss";
            }
            else{
                url.protocol = "ws";
                if (webSocketPortProvider.port >= 0){
                    url.port = webSocketPortProvider.port;
                }
                else{
                    console.error("WebSocket port provider has invalid port!");
                }
            }

            if (context.appName && context.appName !== ""){
                url.pathname = "/" + context.appName + "/wssub";
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

        function onSuperuserExistResult(exists){
            if (exists){
                thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
            }
            else{
                thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
            }
        }

        function onLoginSuccessful(){
            thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator;
            thumbnailDecorator.showPage(undefined)
            Events.sendEvent("UpdateSettings");
        }

        function onLogoutSignal(){
            application.firstModelsInit();
        }
    }

    property Timer timer: Timer{
        id: timer;

        interval: 3000;

        repeat: true;
        running: application.useWebSocketSubscription && webSocketPortProvider.port == -1;

        onTriggered: {
            if (!application.useWebSocketSubscription){
                return;
            }

            if (webSocketPortProvider.port == -1){
                application.checkStatus(4)

                if (application.serverReady){
                    webSocketPortProvider.updateModel();
                }
            }
        }
    }
}


