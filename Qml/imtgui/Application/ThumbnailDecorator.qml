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

    property alias drawingContainer: drawingContainer_;
    property alias stackView: stackView_;
    property alias topPanel: topPanel_;

    property int mainMargin: Style.mainWindowMargin !== undefined ? Style.mainWindowMargin : 0;
    property int pageMargin: Style.pageMargin !== undefined ? Style.pageMargin : 0;
    property int mainRadius: 0;

    property Item dialogManager: modalDialogManager;

    property alias menuPanelRadius: menuPanel.radius;
    property alias loadPageByClick: pagesManager.loadByClick;
    property bool canRecoveryPassword: true;

    property SettingsProvider settingsProvider: null;
    property SettingsObserver settingsObserver: null;

    Component.onCompleted: {
        Events.subscribeEvent("StartLoading", thumbnailDecoratorContainer.startLoading);
        Events.subscribeEvent("StopLoading", thumbnailDecoratorContainer.stopLoading);

        Events.subscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
        Events.subscribeEvent("Logout", thumbnailDecoratorContainer.onLogout);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("StartLoading", thumbnailDecoratorContainer.startLoading);
        Events.unSubscribeEvent("StopLoading", thumbnailDecoratorContainer.stopLoading);

        Events.unSubscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
        Events.unSubscribeEvent("Logout", thumbnailDecoratorContainer.onLogout);
    }

    function updateModels(){
        pagesManager.updateModel();
    }

    function closeAllPages(){
    }

    function onLogout(){
        clearModels();
        drawingContainer.content = null;
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
        anchors.top: topPanel_.bottom;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: bottomPanel.top;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;

        model: pagesManager.pageModel;

        color: !pagesManager.pageModel.GetItemsCount() ? "transparent" : Style.color_menuPanel !==undefined ? Style.color_menuPanel : Style.imagingToolsGradient1;

        Component.onCompleted: {
            MainDocumentManager.documentOpened.connect(onDocumentOpened);
        }

        Component.onDestruction: {
            MainDocumentManager.documentOpened.disconnect(onDocumentOpened);
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

    PagesManager {
        id: pagesManager;

        anchors.left: menuPanel.right;
        anchors.right: thumbnailDecoratorContainer.right;
        anchors.top: topPanel_.bottom;
        anchors.bottom: thumbnailDecoratorContainer.bottom;

        anchors.topMargin: thumbnailDecoratorContainer.pageMargin;
        anchors.bottomMargin: thumbnailDecoratorContainer.pageMargin;
        anchors.leftMargin: thumbnailDecoratorContainer.pageMargin;
        anchors.rightMargin: thumbnailDecoratorContainer.pageMargin;

        activePageIndex: menuPanel.activePageIndex;

        onModelStateChanged: {
//            if (pagesManager.modelState === "Ready"){
//                thumbnailDecoratorContainer.startLoading();
//            }
//            else{
//                thumbnailDecoratorContainer.stopLoading();
//            }
        }
    }

    DrawingContainer{
        id: drawingContainer_;
        z: topPanel_.z + 1;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;
        edge: Qt.RightEdge;
    }

    TopPanel {
        id: topPanel_;

        z: 10;

        anchors.top: parent.top;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;

        width: parent.width;
        height: Style.size_panelsHeight !== undefined ? Style.size_panelsHeight : 60;
    }

    function showPreferencePage(){
        if (thumbnailDecoratorContainer.settingsProvider != null){
            let representationModel = thumbnailDecoratorContainer.settingsProvider.getRepresentationModel();

            if (thumbnailDecoratorContainer.settingsObserver != null){
                thumbnailDecoratorContainer.settingsObserver.registerModel(representationModel);
            }

            modalDialogManager.openDialog(preferenceDialogComp, {"settingsModel": representationModel, "settingsProvider": thumbnailDecoratorContainer.settingsProvider });
        }
    }

    StackView {
        id: stackView_;

        z: 11;

        anchors.fill: parent;
    }

    function showPage(pageComp){
        stackView.pagesModel.clear();

        if (pageComp){
            stackView.push(pageComp)
        }
    }

    property Component authorizationPageComp: Component {
        AuthorizationPage {
            id: authorizationPage;

            anchors.fill: parent;
            anchors.topMargin: topPanel_.height;

            canRecoveryPassword: thumbnailDecoratorContainer.canRecoveryPassword;
        }
    }

    property Component superuserPasswordPageComp: Component {
        SuperuserPasswordPage {
            id: superuserPasswordPage;

            anchors.fill: parent;
            anchors.topMargin: topPanel_.height;

            onBeforeSetted: {
                thumbnailDecoratorContainer.startLoading();
            }

            onVisibleChanged: {
                if (visible){
                    thumbnailDecoratorContainer.stopLoading();
                }
            }

            onPasswordSetted: {
                thumbnailDecoratorContainer.stopLoading();

                thumbnailDecoratorContainer.showPage(thumbnailDecoratorContainer.authorizationPageComp);
            }

            onFailed: {
                thumbnailDecoratorContainer.stopLoading();
                thumbnailDecoratorContainer.showPage(thumbnailDecoratorContainer.superuserPasswordPageComp);
            }
        }
    }

    property Component loadingPageComp: Component {
        Loading {
            id: loading;
            z: 9999;
            anchors.fill: parent;
        }
    }

    function startLoading(){
        stackView.push(loadingPageComp);
    }

    function stopLoading(){
        let index = stackView.getPageIndex(loadingPageComp);
        if (index >= 0){
            stackView.removePageByIndex(index);
        }
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

//        errorPage: serverNoConnectionView;

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
