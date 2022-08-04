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
        console.log("ThumbnailDecorator onLocalSettingsChanged");
        preferenceDialog.localSettings = thumbnailDecoratorContainer.localSettings;
    }

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");
        Style.changeSchemeDesign("");

        menuPanel.updateModels();
        topPanel.updateModels();

        preferenceDialog.loadSettings();
    }

    MenuPanel {
        id: menuPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.top: topPanel.bottom;
        anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: customPanel.top;
        anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;
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
                    console.log("loaderCustomPanel ", Style.customPanelDecoratorPath);
                    customPanel.height = loaderCustomPanel.item.height;
                    customPanel.width = loaderCustomPanel.item.width;
                    console.log("loaderCustomPanel.item.height ", loaderCustomPanel.item.height);
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
        anchors.top: parent.top;

        width: parent.width;
        height: 60;
    }

    function setPreferencesVisible(visible){
        preferenceDialog.visible = visible;
    }

    Rectangle {
        id: background;

        anchors.fill: parent;
        color: "gray";
        visible: false;
        opacity: 0.4;
        MouseArea {
            anchors.fill: parent;
            onWheel: {}
        }
    }

    PreferenceDialog {
        id: preferenceDialog;

        anchors.centerIn: parent;

        visible: false;

        onVisibleChanged: {
            background.visible = preferenceDialog.visible;
        }
    }

    ModalDialogManager {
        id: modalDialogManager;

        anchors.fill: parent;
    }

    ServerConnectionManager{
        id: serverConnectionManager;
        anchors.fill: parent;
    }
}
