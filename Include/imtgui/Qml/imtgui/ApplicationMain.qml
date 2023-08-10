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

    signal settingsUpdate();
    signal localSettingsUpdated();

    onLocalSettingsUpdated: {
        application.updateAllModels();
    }

    Component.onCompleted: {
        Events.subscribeEvent("UpdateModels", application.updateAllModels);

        thumbnailDecorator.userManagementProvider.updated.connect(application.onUserModeChanged);

        thumbnailDecorator.loadingPage.start();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateModels", application.updateAllModels);
    }

    onSystemStatusChanged: {
        console.log("onSystemStatusChanged", application.systemStatus);

        application.run();
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
            serverSettingsObserver.observedModelDataChanged();
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

    function onLocalizationChanged(language){
        console.log("Main onLocalizationChanged", language);
        Events.sendEvent("OnLocalizationChanged", language);
    }

    function updateAllModels(){
        thumbnailDecorator.updateModels();
        applicationInfoProvider.updateModel();
    }

    function onUserModeChanged(){
        application.run();
    }

    function run(){
        console.log("run");

        let userMode = thumbnailDecorator.userManagementProvider.userMode;
        let loggedUserId = thumbnailDecorator.authorizationPageAlias.getLoggedUserId();

        // If no user management - update all model without login
        if (userMode == "NO_USER_MANAGEMENT" || userMode == "OPTIONAL_USER_MANAGEMENT"){
            application.onSimpleUserManagement();
        }
        else if (userMode == "STRONG_USER_MANAGEMENT"){
            application.onStrongUserManagement();
        }
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

        if (application.systemStatus == "NO_ERROR"){
            thumbnailDecorator.errorPage.visible = false;

            // If user is logged in the system - continue, else check supeuser
            if (loggedUserId === ""){
                thumbnailDecorator.superuserProvider.superuserExists();
            }
        }
        else{
            application.showErrorPage(application.message);
        }
    }

    function setSystemStatus(status, message){
        console.log("setSystemStatus", status, message);
        application.message = message;

        application.systemStatus = status;
    }

    function showErrorPage(errorMessage){
        console.log("showErrorPage", errorMessage);

        if (errorMessage && errorMessage !== ""){
            thumbnailDecorator.errorPage.text = errorMessage;
        }

        thumbnailDecorator.errorPage.visible = true;
    }

    function firstModelsInit(){
        console.log("firstModelsInit");
        thumbnailDecorator.userManagementProvider.updateModel();
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
