import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'

Rectangle
{
    id: thubnailDecoratorContainer;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property Item activeItem;
    property TreeItemModel localSettings;
    property bool serverIsConnection: false;

    property int mainMargin: 0;
    property int mainRadius: 0;

    onLocalSettingsChanged: {
        console.log("ThumbnailDecorator onLocalSettingsChanged");
        preferenceDialog.localSettings = thubnailDecoratorContainer.localSettings;
    }

    function setFocusOnMenuPanel(){
        menuPanel.forceActiveFocus();
    }

    function updateModels() {
        console.log("ThumbnailDecorator updateModels()");
        Style.changeSchemeDesign("");

        menuPanel.updateModels();
        topPanel.updateModels();

        preferenceDialog.loadSettings();
    }

    ListModel {
        id: modelLayers;
    }

    function setInvalidConnection(state){
        console.log("ThumbnailDecorator setInvalidConnection", state);
//        thubnailDecoratorContainer.serverIsConnection = !state;
//        refreshButton.enabled = true;
//        errorBackground.visible = state;
    }

    MenuPanel {
        id: menuPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
        anchors.top: topPanel.bottom;
        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
//        anchors.bottom: customPanel.visible ? customPanel.top : parent.bottom;
        anchors.bottom: customPanel.top;
        anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;

        //visible: thubnailDecoratorContainer.serverIsConnection;
        focus: true;

        Keys.onPressed: {
            console.log("ThumbnailDecorator MenuPanel keys pressed")
            if (event.key === Qt.Key_Tab){
                console.log('Key tab was pressed');
                //event.accepted = true;
//                pagesLoader.item.setFocus();
                thubnailDecoratorContainer.activeItem.setFocus();
            }
            else if (event.key === Qt.Key_Up){
                console.log('Key up was pressed');
                if (menuPanel.activePageIndex == 0){
                    menuPanel.activePageIndex = menuPanel.pagesCount - 1;
                }
                else
                    menuPanel.activePageIndex--;
            }
            else if (event.key === Qt.Key_Down){
                console.log('Key down was pressed');
                if (menuPanel.activePageIndex == menuPanel.pagesCount - 1){
                    menuPanel.activePageIndex = 0;
                }
                else
                    menuPanel.activePageIndex++;
            }
        }
    }

    Item {
        id: customPanel;

        anchors.left: parent.left;
        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;

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

    Repeater {
        id: pagesData;

        clip: true;
        model: menuPanel.model;

        delegate: Item {
            id: pagesDeleg;

            anchors.left: menuPanel.right;
            anchors.right: thubnailDecoratorContainer.right;
            anchors.top: topPanel.bottom;
            anchors.bottom: thubnailDecoratorContainer.bottom;

            visible: menuPanel.activePageIndex === model.index;

            /**
                The page will be loaded only by click if it hasn't loaded yet
             */

            onVisibleChanged: {
                if(pagesDeleg.visible){
                    thubnailDecoratorContainer.activeItem = pagesLoader.item;

                    if (!pagesLoader.item){
                        var source = menuPanel.model.GetData("Source", model.index);
                        pagesLoader.source = source;
                    }
                }
            }

            Loader {
                id: pagesLoader;
                anchors.fill: parent;

                onItemChanged: {
                    console.log("ThumbnailDecorator Repeater Loader onItemChanged", pagesLoader.source)
                    if (pagesLoader.item){

                        let startPage = {};
                        startPage["Id"] = menuPanel.model.GetData("PageId", model.index);
                        startPage["Name"] = menuPanel.model.GetData("Name", model.index);
                        startPage["Source"] = menuPanel.model.GetData("StartItem", model.index);
                        startPage["CommandsId"] = menuPanel.model.GetData("PageId", model.index);

                        pagesLoader.item.startPageObj = startPage;
                    }
                }
            }
        }
    }

    Rectangle {
        id: errorBackground;

        anchors.fill: parent;

        color: "transparent";

        visible: false;

        MouseArea {
            anchors.fill: parent;
        }

        Rectangle {
            anchors.verticalCenter: errorBackground.verticalCenter;
            anchors.left: errorBackground.left;
            anchors.right: errorBackground.right;

            height: 300;

            color: Style.backgroundColor;
            opacity: 0.8;

            Text {
                id: textNoConnection;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;

                text: qsTr("There is no connection to the server!");
                color: "red";
                font.pixelSize: Style.fontSize_title;
                font.family: Style.fontFamily;
            }

            AuxButton {
                id: refreshButton;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.bottom: parent.bottom;
                anchors.bottomMargin: 50;

                width: 70;
                height: 25;
                backgroundColor: Style.imagingToolsGradient1;

                hasText: true;
                textButton: qsTr("Refresh");

                onClicked: {
                    refreshButton.enabled = false;
                    thubnailDecoratorContainer.updateModels();
                }
            }
        }
    }

    TopPanel {
        id: topPanel;
        anchors.top: parent.top;

        width: parent.width;
        height: 60;

        title: menuPanel.activePageName;
        onMenuActivatedSignal: {
            thubnailDecoratorContainer.activeItem.menuActivated(idMenu);
        }

        onCommandsChangedSignal: {
            console.log("ThumbnailDecorator TopPanel onCommandsChangedSignal", commandsId);
            thubnailDecoratorContainer.activeItem.commandsChanged(commandsId);
        }

        MouseArea {
            id: rectBlock;

            anchors.horizontalCenter: topPanel.horizontalCenter;

            width: topPanel.width - 200;
            height: topPanel.height;

            visible: errorBackground.visible;
        }
    }


//    Connections {
//        target: Qt.application;

//        onAboutToQuit: {
//            console.log("Quit application");
//            listViewDialogs.destroy();
//        }
//    }

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
}
