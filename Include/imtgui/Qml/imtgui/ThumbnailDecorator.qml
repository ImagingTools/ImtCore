import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'

Rectangle {
    id: thumbnailDecoratorContainer;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property TreeItemModel localSettings;
    property bool serverIsConnection: true;

    property int mainMargin: 0;
    property int mainRadius: 0;

    onLocalSettingsChanged: {
        preferenceDialog.localModel = thumbnailDecoratorContainer.localSettings;
    }

    property alias authorizationPage: authorizationPage;
    property alias preferencePage: preferenceDialog;

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");

        pagesManager.updateModel();
        preferenceDialog.updateModel();
    }

    function clearModels(){
        menuPanel.clearModels();
        pagesManager.clearModels();

        preferenceDialog.clearModels();
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

    PagesManager{
        id: pagesManager;

        anchors.left: menuPanel.right;
        anchors.right: thumbnailDecoratorContainer.right;
        anchors.top: topPanel.bottom;
        anchors.bottom: thumbnailDecoratorContainer.bottom;

        pageModel: menuPanel.model;

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

        visible: false;

        onAccepted: {
            visible = false;

            updateAllModels();
        }
    }

    PreferencePage {
        id: preferenceDialog;

        z: 20;

        anchors.fill: parent;

        root: window;

        visible: false;
    }

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
    }
}
