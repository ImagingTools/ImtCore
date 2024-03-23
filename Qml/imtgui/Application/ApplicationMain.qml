import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0


Item {
    id: application;

    anchors.fill: parent;

    property Decorators decorators: decorators_

    property string message;
//    property string systemStatus: "NO_ERROR";
    property alias localSettings: application.settingsProvider.localModel;

    property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;

    property string applicationId;
    property var applicationInfo;

    property bool serverReady: false;
    property bool useWebSocketSubscription: false;

    property alias loadPageByClick: thumbnailDecorator.loadPageByClick;
    property alias canRecoveryPassword: thumbnailDecorator.canRecoveryPassword;

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
            startSystemStatusChecking();
        }
    }

    signal updateSystemStatus();
    signal settingsUpdate();
    signal localSettingsUpdated();

    onLocalSettingsUpdated: {
        application.updateAllModels();
    }

    Connections {
        target: application.settingsProvider;

        function onLocalModelChanged(){
            let serverUrl = application.settingsProvider.getValue("ServerUrl");

            application.systemStatusController.serverUrl = serverUrl;
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

    Component.onCompleted: {
        setDecorators()

        Events.subscribeEvent("UpdateModels", application.updateAllModels);
        Events.subscribeEvent("Logout", application.onLogout);
        Events.subscribeEvent("UpdateSystemStatus", application.updateSystemStatus);

        thumbnailDecorator.userManagementProvider.updated.connect(application.onUserModeChanged);

        thumbnailDecorator.loadingPage.start();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateModels", application.updateAllModels);
        Events.unSubscribeEvent("Logout", application.onLogout);
        Events.unSubscribeEvent("UpdateSystemStatus", application.updateSystemStatus);
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

//        onLocalModelChanged: {
//            application.designProvider.applyCachedDesignSchema();
//        }

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

    property SystemStatusController systemStatusController : SystemStatusController{
        onSystemStatusChanged: {
            console.log("AppMain onSystemStatusChanged", systemStatus);

            let message = this.getLastMessage();

            thumbnailDecorator.closeAllPages();
            thumbnailDecorator.messagePage.loadingVisible = false;

            if (systemStatus === 1){
                thumbnailDecorator.messagePage.loadingVisible = true;
            }

            if (systemStatus === 0 ||
                systemStatus === 1 ||
                systemStatus === 2 ||
                systemStatus === 3){
                thumbnailDecorator.messagePage.visible = true;
                thumbnailDecorator.messagePage.text = message
            }
            else if (systemStatus === 4){
                thumbnailDecorator.messagePage.visible = false;

                // No error
                let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();
                if (loggedUserId === ""){
                    thumbnailDecorator.closeAllPages();

                    application.firstModelsInit();
                }
            }
        }
    }

    SubscriptionManager {
        id: subscriptionManager;

        active: application.useWebSocketSubscription;

        property bool applyUrl: application.serverReady && application.settingsProvider.serverModel != null;
        onApplyUrlChanged: {
            console.log("onApplyUrlChanged", applyUrl);

            if (applyUrl){
                let webSocketServerUrl = application.settingsProvider.getValue("WebSocketServerUrl");
                if (webSocketServerUrl && webSocketServerUrl !== ""){
                    webSocketServerUrl =  webSocketServerUrl.replace("http", "ws")
                    subscriptionManager.url = webSocketServerUrl;

                    return;
                }

                let wsUrl = "";
                let serverUrl = application.settingsProvider.getValue("ServerUrl");
                if (serverUrl !== ""){
                    wsUrl = application.getWebSocketUrl(serverUrl);
                }
                else{
                    wsUrl = application.getWebSocketUrl(context.location);
                }

                console.log("WEB Socket serverUrl", wsUrl);
                subscriptionManager.url = wsUrl;
            }
        }

        onError: {
            Events.sendEvent("SendWarningError", qsTr("There is no connection to the subscription server. Check the Web Server Socket Url in the settings or contact your system administrator."));

            application.systemStatusController.updateSystemStatus();
        }
    }

    WebSocketPortProvider {
        id: webSocketPortProvider;
    }

    function startSystemStatusChecking(){
        systemStatusController.updateSystemStatus();
    }

    function getWebSocketUrl(serverUrl){
        console.log("getWebSocketUrl", serverUrl);

        try {
            let url = new URL(serverUrl);

            url.protocol = "ws";

            console.log("context.appName", context.appName);


            if (context.appName && context.appName !== ""){
                console.log("===");

                url.pathname = "/" + context.appName + "/wssub";
            }

            if (webSocketPortProvider.port >= 0){
                url.port = webSocketPortProvider.port;
            }
            else{
                console.error("WebSocket port provider has invalid port!");
            }

            console.log("url.pathname", url.pathname);

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

        subscriptionManager.clear();
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
}
