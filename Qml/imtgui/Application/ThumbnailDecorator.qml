import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: thumbnailDecoratorContainer;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property alias errorPage: serverNoConnectionView;
    property alias loadingPage: loading;

    property int mainMargin: Style.mainWindowMargin !== undefined ? Style.mainWindowMargin : 0;
    property int pageMargin: Style.pageMargin !== undefined ? Style.pageMargin : 0;
    property int mainRadius: 0;

    property alias authorizationPageAlias: authorizationPage;
    property alias superuserPasswordPageAlias: superuserPasswordPage;

    property alias preferencePage: preferenceDialog;
    property alias userManagementProvider: userManagement;
//    property alias documentManager: mainDocumentManager;
    property Item dialogManager: modalDialogManager;

    property alias menuPanelRadius: menuPanel.radius;
    property alias loadPageByClick: pagesManager.loadByClick;
    property alias canRecoveryPassword: authorizationPage.canRecoveryPassword;

    property var applicationMain: null;

    property SettingsProvider settingsProvider: null;
    property SettingsObserver settingsObserver: null;

    Component.onCompleted: {
        Events.subscribeEvent("StartLoading", thumbnailDecoratorContainer.loadingPage.start);
        Events.subscribeEvent("StopLoading", loading.stop);

        Events.subscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
        Events.subscribeEvent("Logout", thumbnailDecoratorContainer.onLogout);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("StartLoading", thumbnailDecoratorContainer.loadingPage.start);
        Events.unSubscribeEvent("StopLoading", thumbnailDecoratorContainer.loadingPage.stop);

        Events.unSubscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
        Events.unSubscribeEvent("Logout", thumbnailDecoratorContainer.onLogout);
    }

    property SuperuserProvider superuserProvider : SuperuserProvider {
        onResult: {
            if (exists){
                thumbnailDecoratorContainer.closeAllPages();

                authorizationPage.visible = true;
            }
            else{
                if (error === ""){
                    superuserPasswordPage.visible = true;
                }
                else{
                    Events.sendEvent("SendCriticalError", error);
                }
            }
        }

        onModelStateChanged: {
            if (state === "Ready"){
                thumbnailDecoratorContainer.loadingPage.stop();
            }
            else{
                 thumbnailDecoratorContainer.loadingPage.start();
            }
        }
    }

    function updateModels(){
        pagesManager.updateModel();
    }

    function closeAllPages(){
        authorizationPage.visible = false;
        superuserPasswordPage.visible = false;
        thumbnailDecoratorContainer.loadingPage.visible = false;
        serverNoConnectionView.visible = false;
        preferenceDialog.visible = false;
    }

    function onLogout(){
        clearModels();
        authorizationPage.logout();
// soon...
//        let exists = mainDocumentManager.dirtyDocumentsExists();
//        if (exists){
//            modalDialogManager.openDialog(saveDialog, {});
//        }
//        else{
//            clearModels();
//            authorizationPage.logout();
//        }
    }

    function clearModels(){
        menuPanel.clearModels();
        pagesManager.clearModels();

        preferenceDialog.clearModels();

        if (settingsProvider){
            settingsProvider.clearModel();
            settingsProvider.serverModel = null;
        }

        MainDocumentManager.clear();
    }

    MenuPanel {
        id: menuPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.top: topPanel.bottom;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: bottomPanel.top;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;

        model: pagesManager.pageModel;

        color: Style.color_menuPanel !==undefined ? Style.color_menuPanel : Style.imagingToolsGradient1;

        Component.onCompleted: {
            MainDocumentManager.documentOpened.connect(onDocumentOpened);
        }

        function onDocumentOpened(typeId, documentId, documentTypeId){
            for (let i = 0; i < menuPanel.model.GetItemsCount(); i++){
                let pageId = menuPanel.model.GetData("Id", i);
                if (pageId === typeId){
                    menuPanel.activePageIndex = i;
                    break;
                }
            }
        }
    }

    BottomPanel {
        id: bottomPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;
    }

//    MainDocumentManager {
//        id: mainDocumentManager;

//        onDocumentOpened: {
//            for (let i = 0; i < menuPanel.model.GetItemsCount(); i++){
//                let pageId = menuPanel.model.GetData("Id", i);
//                if (pageId === typeId){
//                    menuPanel.activePageIndex = i;
//                    break;
//                }
//            }
//        }
//    }

    PagesManager {
        id: pagesManager;

        anchors.left: menuPanel.right;
        anchors.right: thumbnailDecoratorContainer.right;
        anchors.top: topPanel.bottom;
        anchors.bottom: thumbnailDecoratorContainer.bottom;

        anchors.topMargin: thumbnailDecoratorContainer.pageMargin;
        anchors.bottomMargin: thumbnailDecoratorContainer.pageMargin;
        anchors.leftMargin: thumbnailDecoratorContainer.pageMargin;
        anchors.rightMargin: thumbnailDecoratorContainer.pageMargin;

        activePageIndex: menuPanel.activePageIndex;

        authorizationStatusProvider: authorizationPage;

        onModelStateChanged: {
            if (pagesManager.modelState === "Ready"){
                thumbnailDecoratorContainer.loadingPage.stop();
            }
            else{
//                 thumbnailDecoratorContainer.loadingPage.start();
            }
        }
    }

    TopPanel {
        id: topPanel;

        z: 10;

        anchors.top: parent.top;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;

        width: parent.width;
        height: Style.size_panelsHeight !== undefined ? Style.size_panelsHeight : 60;
    }

    ServerNoConnectionView {
        id: serverNoConnectionView;

        z: 5;

        anchors.fill: parent;

        visible: false;

        onVisibleChanged: {
            Events.sendEvent("SetCommandsVisible", !visible);
            Events.sendEvent("SetUserPanelVisible", !visible);
        }

        onRefresh: {
            if (thumbnailDecoratorContainer.applicationMain != null){
                thumbnailDecoratorContainer.applicationMain.updateSystemStatus();
            }
        }
    }

    function showPreferencePage(){
        if (thumbnailDecoratorContainer.settingsProvider != null){
            let representationModel = thumbnailDecoratorContainer.settingsProvider.getRepresentationModel();

            if (thumbnailDecoratorContainer.settingsObserver != null){
                thumbnailDecoratorContainer.settingsObserver.registerModel(representationModel);
            }

            modalDialogManager.openDialog(preferenceDialogComp, {"settingsModel": representationModel, "settingsProvider": settingsProvider });
        }
    }

    AuthorizationPage {
        id: authorizationPage;

        anchors.fill: parent;
        anchors.topMargin: topPanel.height;

        visible: false;

        onLoginSuccessful: {
            authorizationPage.visible = false;
            authorizationPage.state = "authorized";

            Events.sendEvent("UpdateSettings");
            Events.sendEvent("UpdateModels");
        }
    }

    SuperuserPasswordPage {
        id: superuserPasswordPage;

        anchors.fill: parent;

        visible: false;

        onBeforeSetted: {
             thumbnailDecoratorContainer.loadingPage.start();
        }

        onVisibleChanged: {
            if (visible){
                thumbnailDecoratorContainer.loadingPage.visible = false;
            }
        }

        onPasswordSetted: {
            thumbnailDecoratorContainer.loadingPage.stop();

            superuserPasswordPage.visible = false;
            authorizationPage.visible = true;
        }

        onFailed: {
            thumbnailDecoratorContainer.loadingPage.stop();

            thumbnailDecoratorContainer.closeAllPages();
        }
    }

    Loading {
        id: loading;

        z: 9999;

        anchors.fill: parent;

        visible: false;
    }

    Component {
        id: preferenceDialogComp;
        PreferenceDialog {
            onFinished: {
                if (buttonId == Enums.apply){
                    if (thumbnailDecoratorContainer.settingsObserver != null){
                        thumbnailDecoratorContainer.settingsObserver.observedModelDataChanged();
                    }
                }
            }
        }
    }

    Preference {
        id: preferenceDialog;

        z: 20;

        anchors.fill: parent;

        visible: false;

        onVisibleChanged: {
            if (visible){
                if (thumbnailDecoratorContainer.settingsProvider != null){
                    let representationModel = thumbnailDecoratorContainer.settingsProvider.getRepresentationModel();

                    preferenceDialog.settingsModel = representationModel;
                }
            }
            else{
                preferenceDialog.clearModels();
            }
        }
    }

    GqlModelObserver {
        observedModel: userManagement.userModeGqlModel;

        noConnectionView: serverNoConnectionView;
    }

    UserManagementProvider {
        id: userManagement;
    }

    ShortcutManager {
        id: shortcutManager;
    }

    property Component errorDialog: Component {
        ErrorDialog {
            onFinished: {}
        }
    }

    ErrorManager {
        id: errorManager;

        anchors.fill: parent;

        errorPage: serverNoConnectionView;

        visible: false;
    }

    // TODO
//    function closeApp(){
//        if (mainDocumentManager.dirtyDocumentsExists()){
//            modalDialogManager.openDialog(saveDialog, {});
//        }
//    }

//    Component {
//        id: saveDialog;
//        MessageDialog {
//            title: qsTr("Save dirty documents");
//            message: qsTr("Save all dirty documents ?");
//            Component.onCompleted: {
//                buttonsModel.append({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true});
//            }

//            onFinished: {
//                if (buttonId == "Yes"){
//                    documentManager.saveDirtyDocuments();
//                }
//                else if (buttonId == "No"){
//                    documentManager.closeAllDocuments();
//                }
//            }
//        }
//    }
}
