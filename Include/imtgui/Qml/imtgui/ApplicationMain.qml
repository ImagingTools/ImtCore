import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: application;

    anchors.fill: parent;

    property string message;
    property string systemStatus: "NO_ERROR";
    property alias localSettings: application.settingsProvider.localModel;

    property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;

    property var applicationInfo;

    property bool serverReady: false;
    property bool useWebSocketSubscription: false;

    onApplicationInfoChanged: {
        console.log("onApplicationInfoChanged");
        if (application.applicationInfo){
            applicationInfoProvider.clientApplicationInfo = application.applicationInfo;
        }
    }

    signal updateSystemStatus();
    signal settingsUpdate();
    signal localSettingsUpdated();

    onUpdateSystemStatus: {
        console.log("AppMain onUpdateSystemStatus");
    }

    onLocalSettingsUpdated: {
        application.updateAllModels();
    }

    onSystemStatusChanged: {
        Events.sendEvent("SystemStatusChanged", application.systemStatus)
    }

    Component.onCompleted: {
        console.log("AppMain onCompleted");

        Events.subscribeEvent("UpdateModels", application.updateAllModels);
//        Events.subscribeEvent("Logout", application.onLogout);
        Events.subscribeEvent("UpdateSystemStatus", application.updateSystemStatus);

        thumbnailDecorator.userManagementProvider.updated.connect(application.onUserModeChanged);

        thumbnailDecorator.loadingPage.start();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateModels", application.updateAllModels);
//        Events.unSubscribeEvent("Logout", application.onLogout);
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
            let design = application.designProvider.getDesignSchema();

            console.log("design", design);

            let index = application.designProvider.getDesignSchemaIndex(design);
            if (index >= 0){
                application.settingsObserver.onDesignSchemaChanged(index);
            }
        }

        onLocalModelChanged: {
            application.designProvider.applyCachedDesignSchema();
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
        id: subscriptionManager;

        active: application.useWebSocketSubscription;

        property bool applyUrl: application.serverReady && application.settingsProvider.serverModel != null;
        onApplyUrlChanged: {
            if (applyUrl){
                console.log("onApplyUrlChanged");
                let webSocketServerUrl = application.settingsProvider.getValue("WebSocketServerUrl");
                console.log("webSocketServerUrl", webSocketServerUrl);
                if (webSocketServerUrl && webSocketServerUrl !== ""){
                    webSocketServerUrl =  webSocketServerUrl.replace("http", "ws")
                    subscriptionManager.url = webSocketServerUrl;

                    return;
                }

                let serverUrl = application.settingsProvider.getValue("ServerUrl");

                serverUrl = serverUrl.replace("http", "ws")

                if (serverUrl[serverUrl.length - 1] !== '/'){
                    serverUrl += "/";
                }

                console.log("serverUrl", serverUrl);

//                serverUrl += "Lisa/wssub";

                subscriptionManager.url = serverUrl;
            }
        }

        onError: {
            Events.sendEvent("SendWarningError", qsTr("There is no connection to the subscription server. Check the Web Server Socket Url in the settings or contact your system administrator."));
        }
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;

        anchors.fill: parent;

        settingsProvider: application.settingsProvider;
        settingsObserver: application.settingsObserver;

        applicationMain: application;
    }

//    function onLogout(){
//        settingsProvider.serverModel = null;
//    }

    function onLocalizationChanged(language){
        console.log("Main onLocalizationChanged", language);
       // Events.sendEvent("OnLocalizationChanged", language);
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

    function setSystemStatus(status, message){
        console.log("setSystemStatus", status, message);
        application.message = message;
        application.systemStatus = status;

        if (application.systemStatus == "NO_ERROR"){
            thumbnailDecorator.errorPage.visible = false;

            let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();
            if (loggedUserId === ""){
                thumbnailDecorator.closeAllPages();

                firstModelsInit();
            }
        }
        else if (application.systemStatus == "UNKNOWN_ERROR"){

        }
        else if (application.systemStatus == "CONNECTION_ERROR"){
            thumbnailDecorator.closeAllPages();

            thumbnailDecorator.errorPage.text = message;
            thumbnailDecorator.errorPage.visible = true;
        }
        else if (application.systemStatus == "DATABASE_CONNECTION_ERROR"){
            thumbnailDecorator.closeAllPages();

            thumbnailDecorator.errorPage.text = message;
            thumbnailDecorator.errorPage.visible = true;
        }
        else if (application.systemStatus == "TRY_CONNECTING"){
            thumbnailDecorator.closeAllPages();

            thumbnailDecorator.errorPage.text = message;
            thumbnailDecorator.errorPage.visible = true;
        }
    }

    function firstModelsInit(){
        console.log("firstModelsInit");
        if (application.systemStatus == "NO_ERROR"){
            thumbnailDecorator.userManagementProvider.updateModel();
        }
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
//    Connections {
//        target: Qt.application;

//        onAboutToQuit: {
//            console.log("onAboutToQuit");

//            let dirtyDocumentsExists = thumbnailDecorator.documentManager.dirtyDocumentsExists();

//            console.log("dirtyDocumentsExists", dirtyDocumentsExists);
//        }
//    }
}
