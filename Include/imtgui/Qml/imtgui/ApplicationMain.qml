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

//    property alias applicationInfo: applicationInfoProvider.clientApplicationInfo;
    property var applicationInfo;

    property bool serverReady: false;

    onApplicationInfoChanged: {
        console.log("onApplicationInfoChanged");
        if (application.applicationInfo){
            applicationInfoProvider.clientApplicationInfo = application.applicationInfo;
        }
    }

    signal settingsUpdate();
    signal localSettingsUpdated();

    onLocalSettingsUpdated: {
        application.updateAllModels();
    }

    onSystemStatusChanged: {
        Events.sendEvent("SystemStatusChanged", application.systemStatus)
    }

    Component.onCompleted: {
        console.log("AppMain onCompleted");

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
            if (applyCachedLanguage){
                let lang = application.languageProvider.getLanguage();
                context.language = lang;
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

    ThumbnailDecorator {
        id: thumbnailDecorator;

        anchors.fill: parent;

        settingsProvider: application.settingsProvider;
        settingsObserver: application.settingsObserver;
    }

    function onLogout(){
        settingsProvider.serverModel = null;
    }

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
