import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtguigql 1.0
import 'AuxComponents'

Rectangle {
    id: thumbnailDecoratorContainer;

    anchors.fill: parent;

    color: Style.backgroundColor;

//    property alias settingsProvider: preferenceDialog.settingsProvider;
//    property alias applicationInfoProvider: preferenceDialog.applicationInfoProvider;

    property alias errorPage: serverNoConnectionView;
    property alias loadingPage: loading;

    property int mainMargin: Style.mainWindowMargin !== undefined ? Style.mainWindowMargin : 0;
    property int mainRadius: 0;

    property alias authorizationPageAlias: authorizationPage;
    property alias superuserPasswordPageAlias: superuserPasswordPage;

    property alias preferencePage: preferenceDialog;
    property alias userManagementProvider: userManagement;
    property alias documentManager: mainDocumentManager;
    property alias dialogManager: modalDialogManager;

    property var applicationMain: null;

    property SettingsProvider settingsProvider: null;
    property SettingsObserver settingsObserver: null;

    Component.onCompleted: {
        Events.subscribeEvent("StartLoading", loading.start);
        Events.subscribeEvent("StopLoading", loading.stop);

        Events.subscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
        Events.subscribeEvent("Logout", thumbnailDecoratorContainer.onLogout);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("StartLoading", loading.start);
        Events.unSubscribeEvent("StopLoading", loading.stop);

        Events.unSubscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
        Events.unSubscribeEvent("Logout", thumbnailDecoratorContainer.onLogout);
    }

    property SuperuserProvider superuserProvider : SuperuserProvider {
        onResult: {
            console.log("SuperuserProvider onResult", exists);
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
            console.log("Superuser onModelStateChanged", state);

            if (state === "Ready"){
                console.log("SuperuserProvider Ready", state);
                loading.stop();
            }
            else{
                loading.start();
            }
        }
    }

    function updateModels(){
        pagesManager.updateModel();
    }

    function closeAllPages(){
        authorizationPage.visible = false;
        superuserPasswordPage.visible = false;
        loading.visible = false;
        serverNoConnectionView.visible = false;
        preferenceDialog.visible = false;
    }

    function onLogout(){
        clearModels();
        authorizationPage.logout();
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
        settingsProvider.clearModel();

        mainDocumentManager.documentManagers = {}
    }

    MenuPanel {
        id: menuPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.top: topPanel.bottom;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: customPanel.top;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;

        model: pagesManager.pageModel;

        color: Style.color_menuPanel !==undefined ? Style.color_menuPanel : Style.imagingToolsGradient1;
    }

    Item {
        id: customPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;

        Loader {
            id: loaderCustomPanel;

            source: Style.customPanelDecoratorPath;

            onItemChanged: {
                if (loaderCustomPanel.item){
                    customPanel.height = loaderCustomPanel.item.height;
                    customPanel.width = loaderCustomPanel.item.width;
                }
            }
        }
    }

    MainDocumentManager {
        id: mainDocumentManager;

        menuPanelRef: menuPanel;
    }

    PagesManager {
        id: pagesManager;

        anchors.left: menuPanel.right;
        anchors.right: thumbnailDecoratorContainer.right;
        anchors.top: topPanel.bottom;
        anchors.bottom: thumbnailDecoratorContainer.bottom;

        activePageIndex: menuPanel.activePageIndex;

        documentManager: mainDocumentManager;

        authorizationStatusProvider: authorizationPage;

        onModelStateChanged: {
            console.log("PagesManager onModelStateChanged", pagesManager.modelState);
            if (pagesManager.modelState === "Ready"){
                console.log("PagesManager Ready", pagesManager.modelState);
                loading.stop();
            }
            else{
                loading.start();
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
        anchors.topMargin: 60;

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
            loading.start();
        }

        onPasswordSetted: {
            console.log("onPasswordSetted Ready");

            loading.stop();

            superuserPasswordPage.visible = false;
            authorizationPage.visible = true;
        }

        onFailed: {
            console.log("onFailed Ready");
//            loading.stop();

            closeAllPages();
        }
    }

    Component {
        id: preferenceDialogComp;
        PreferenceDialog {
            onFinished: {
                if (buttonId == "Apply"){
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

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
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

    Loading {
        id: loading;

        z: 100;

        anchors.fill: parent;

        visible: false;
    }

//    Component {
//        id: saveDialog;
//        MessageDialog {
//            title: qsTr("Save dirty documents");
//            message: qsTr("Save all dirty documents ?");
//            Component.onCompleted: {
//                buttons.addButton({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true});
//            }

//            onFinished: {
//                if (buttonId == "Yes"){
//                    documentManager.saveDirtyDocuments();
//                }
//                else if (buttonId == "No"){
//                    documentManager.closeAllDocuments();

//                   // onLogout();
//                }
//            }
//        }
//    }
}
