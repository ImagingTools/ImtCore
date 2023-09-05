import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: application;

    anchors.fill: parent;

    property string message;
    property string systemStatus: "UNKNOWN_ERROR";
    property alias localSettings: application.settingsProvider.localModel;

//    property alias applicationInfo: applicationInfoProvider.clientApplicationInfo;
    property var applicationInfo;

    onApplicationInfoChanged: {
        if (application.applicationInfo){
            applicationInfoProvider.clientApplicationInfo = application.applicationInfo;
        }
    }

    signal settingsUpdate();
    signal localSettingsUpdated();

    onLocalSettingsUpdated: {
        application.updateAllModels();
    }

    Component.onCompleted: {
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

    property SettingsProvider settingsProvider : SettingsProvider
    {
        onServerModelChanged: {
            application.serverSettingsObserver.registerModel(serverModel);

            console.log("context.language", context.language, context.application);
            context.language = application.languageProvider.getLanguage();
            console.log("context.language", context.language, context.application);

            application.designProvider.applyDesignSchema();
        }

        onLocalModelChanged: {
            localSettingsObserver.registerModel(localModel);
            timer.start();
        }

        onServerSettingsSaved: {
            application.serverSettingsObserver.observedModelDataChanged();
        }

        onLocalSettingsSaved: {
            application.settingsUpdate();
        }
    }

    // Timer for updating design schema when start application, without this timer request does not come
    Timer {
        id: timer;

        interval: 100;

        onTriggered: {
            designProvider.applyDesignSchema();
        }
    }

    property ServerSettingsModelObserver serverSettingsObserver : ServerSettingsModelObserver
    {
        designProvider: application.designProvider;
        languageProvider: application.languageProvider;
    }

    property LocalSettingsModelObserver localSettingsObserver : LocalSettingsModelObserver
    {
        designProvider: application.designProvider;
        languageProvider: application.languageProvider;
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
        applicationInfoProvider: application.applicationInfoProvider;
    }

    function onLogout(){
//        application.applicationInfoProvider.serverApplicationInfo = null;
    }

    function onLocalizationChanged(language){
        console.log("Main onLocalizationChanged", language);
        Events.sendEvent("OnLocalizationChanged", language);
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

            thumbnailDecorator.errorPage.text = qsTr("Server connection error. Please check the server URL in the settings.");
            thumbnailDecorator.errorPage.visible = true;
        }
        else if (application.systemStatus == "DATABASE_CONNECTION_ERROR"){
            thumbnailDecorator.closeAllPages();

            thumbnailDecorator.errorPage.text = message;
            thumbnailDecorator.errorPage.visible = true;
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
//    Connections {
//        target: Qt.application;

//        onAboutToQuit: {
//            console.log("onAboutToQuit");

//            let dirtyDocumentsExists = thumbnailDecorator.documentManager.dirtyDocumentsExists();

//            console.log("dirtyDocumentsExists", dirtyDocumentsExists);
//        }
//    }
}
