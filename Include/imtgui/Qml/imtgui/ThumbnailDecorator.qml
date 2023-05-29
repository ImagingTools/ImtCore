import QtQuick 2.12
import Acf 1.0
import 'AuxComponents'

Rectangle {
    id: thumbnailDecoratorContainer;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property Item root: null;
    property alias settingsProvider: preferenceDialog.settingsProvider;
    property alias applicationInfoProvider: preferenceDialog.applicationInfoProvider;

    property int mainMargin: 0;
    property int mainRadius: 0;

    property alias authorizationPageAlias: authorizationPage;
    property alias preferencePage: preferenceDialog;
    property alias userManagementProvider: userManagement;
    property alias documentManager: mainDocumentManager;
    property alias dialogManager: modalDialogManager;

    Component.onCompleted: {
        Events.subscribeEvent("setPreferencesVisible", thumbnailDecoratorContainer.setPreferencesVisible);
        Events.subscribeEvent("clearModels", thumbnailDecoratorContainer.clearModels);
    }

    function updateModels(){
        pagesManager.updateModel();
    }

    function clearModels(){
        menuPanel.clearModels();
        pagesManager.clearModels();

        preferenceDialog.clearModels();
        settingsProvider.clearModel();

        Events.sendEvent("CommandsDecoratorClear");
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

        color: Style.imagingToolsGradient1;

        onActivePageIndexChanged: {
            //pagesManager.activePageIndex = menuPanel.activePageIndex;
        }
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
    }

    TopPanel {
        id: topPanel;

        z: 10;

        anchors.top: parent.top;

        width: parent.width;
        height: 60;
    }

    ServerNoConnectionView {
        id: serverNoConnectionView;

        z: 5;

        anchors.fill: parent;

        visible: false;

        onRefresh: {
            userManagement.updateModel();
        }
    }

    function setPreferencesVisible(visible){
        preferenceDialog.visible = visible;
    }

    AuthorizationPage {
        id: authorizationPage;

        anchors.fill: parent;
        anchors.topMargin: 60;
        windows: thumbnailDecoratorContainer.root;

        visible: false;
    }

    PreferencePage {
        id: preferenceDialog;

        z: 20;

        anchors.fill: parent;

        root: thumbnailDecoratorContainer.root;

        visible: false;
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

        onUserModeChanged: {
            if (userMode == "NO_USER_MANAGEMENT" || userMode == "OPTIONAL_USER_MANAGEMENT"){
                thumbnailDecoratorContainer.root.updateAllModels();
            }
            else if (userMode == "STRONG_USER_MANAGEMENT"){
                authorizationPage.visible = true;
            }
        }
    }

    ShortcutManager {
        id: shortcutManager;
    }
}
