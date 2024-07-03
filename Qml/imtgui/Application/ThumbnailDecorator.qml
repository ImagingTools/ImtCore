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

    property Item dialogManager: ModalDialogManager;

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

    function onLogout(){
        clearModels();
        Events.sendEvent("DrawerSetVisible", false);
        drawingContainer.content = null;
// soon...
//        let exists = mainDocumentManager.dirtyDocumentsExists();
//        if (exists){
//            ModalDialogManager.openDialog(saveDialog, {});
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

        color: !pagesManager.pageModel.getItemsCount() ? "transparent" : Style.color_menuPanel !==undefined ? Style.color_menuPanel : Style.imagingToolsGradient1;

        Component.onCompleted: {
            MainDocumentManager.documentOpened.connect(onDocumentOpened);
            Events.subscribeEvent("GlobalSearchActivated", seacrhActivated)
        }

        Component.onDestruction: {
            MainDocumentManager.documentOpened.disconnect(onDocumentOpened);
            Events.unSubscribeEvent("GlobalSearchActivated", seacrhActivated)
        }

        function onDocumentOpened(typeId, documentId, documentTypeId){
            for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
                let pageId = menuPanel.model.getData("Id", i);
                if (pageId === typeId){
                    menuPanel.activePageIndex = i;
                    break;
                }
            }
        }

        function seacrhActivated(){
            for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
                let pageId = menuPanel.model.getData("Id", i);
                if (pageId === "Search"){
                    menuPanel.activePageIndex = i;
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

            ModalDialogManager.openDialog(preferenceDialogComp, {"settingsModel": representationModel, "settingsProvider": thumbnailDecoratorContainer.settingsProvider });
        }
    }

    StackView {
        id: stackView_;

        z: topPanel_.z + 1;

        anchors.fill: parent;
    }

    DialogManagerView {
        anchors.fill: parent;

        z: topPanel_.z + 1;
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

            Component.onCompleted: {
                Events.sendEvent("SetUserPanelEnabled", false);
            }
        }
    }

    property Component superuserPasswordPageComp: Component {
        SuperuserPasswordPage {
            id: superuserPasswordPage;

            anchors.fill: parent;
            anchors.topMargin: topPanel_.height;

            Component.onCompleted: {
                Events.sendEvent("SetUserPanelEnabled", false);
            }

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

    Loader {
        id: loadingPageLoader;
        z: 9999;
        anchors.fill: parent;
    }

    property Component loadingPageComp: Component {
        Loading {
            id: loading;
            z: 9999;
            anchors.fill: parent;
        }
    }

    function startLoading(){
        loadingPageLoader.sourceComponent = loadingPageComp;
    }

    function stopLoading(){
        loadingPageLoader.sourceComponent = undefined;
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
}
