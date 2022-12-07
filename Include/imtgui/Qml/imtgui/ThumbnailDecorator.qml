import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'

Rectangle {
    id: thumbnailDecoratorContainer;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property bool serverIsConnection: true;
    property Item root;

    property int mainMargin: 0;
    property int mainRadius: 0;

    property alias authorizationPage: authorizationPage;
    property alias preferencePage: preferenceDialog;
    property alias userManagementProvider: userManagement;

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");

        pagesManager.updateModel();
    }

    function clearModels(){
        console.log("ThumbnailDecorator clearModels()");

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

    PagesManager {
        id: pagesManager;

        anchors.left: menuPanel.right;
        anchors.right: thumbnailDecoratorContainer.right;
        anchors.top: topPanel.bottom;
        anchors.bottom: thumbnailDecoratorContainer.bottom;

        activePageIndex: menuPanel.activePageIndex;
    }

    TopPanel {
        id: topPanel;

        z: 10;

        anchors.top: parent.top;

        width: parent.width;
        height: 60;
    }

    ServerConnectionManager {
        id: serverConnectionManager;

        z: 5;

        anchors.fill: parent;
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

    PermissionsProvider {
        id: permissionsProvider;
    }

    PreferencePage {
        id: preferenceDialog;

        z: 20;

        anchors.fill: parent;

        root: window;

        settingsProvider: window.settingsProvider;

        visible: false;
    }

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
    }

    UserManagementProvider {
        id: userManagement;

        onUserModeChanged: {
            console.log('DEBUG::userManagementProvider')
            if (userMode == "NO_USER_MANAGEMENT"){
                updateAllModels();
            }
            else if (userMode == "STRONG_USER_MANAGEMENT"){
                authorizationPage.visible = true;
            }
        }
    }
}
