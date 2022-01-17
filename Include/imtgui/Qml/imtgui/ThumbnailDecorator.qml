import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
import imtauthgui 1.0
import imtlicgui 1.0

Rectangle
{
    id: thubnailDecoratorContainer;
    anchors.fill: parent;
    width: 300;
    height: 200;
   // color: "white";
    color: Style.baseColor;
    property Item activeItem;

    function updateModels() {
        console.log("updateModels");
        menuPanel.updateModels();
    }

    TopPanel {
        id: topPanel;
        title: menuPanel.activePageName;
        onMenuActivatedSignal: {
            console.log("onMenuActivatedSignal1",menuId, thubnailDecoratorContainer.activeItem)
            thubnailDecoratorContainer.activeItem.menuActivated(menuId);
        }

        onCommandsChangedSignal: {
            console.log("TopPanel onCommandsChangedSignal!", commandsId);
            thubnailDecoratorContainer.activeItem.commandsChanged(commandsId);
        }
    }


    MenuPanel {
        id: menuPanel;

        anchors.top: topPanel.bottom;
        anchors.bottom: parent.bottom;
        onPagesCountChanged: {
            pagesData.model = menuPanel.pagesCount
        }

        onActivePageIdChanged: {
            if (thubnailDecoratorContainer.activeItem){
                thubnailDecoratorContainer.activeItem.updateCommandId();
            }
        }
    }


    Repeater {
        id: pagesData;
        clip: true;
        model: menuPanel.model;

        delegate: Rectangle {
            id: pagesDeleg;
            anchors.left: menuPanel.right;
            anchors.right: thubnailDecoratorContainer.right;
            anchors.top: topPanel.bottom;
            anchors.bottom: thubnailDecoratorContainer.bottom;
            color: "transparent";
            visible: menuPanel.activePageIndex === model.index;
            onVisibleChanged: {
                if(pagesDeleg.visible){
                    thubnailDecoratorContainer.activeItem = loader.item;
                    loader.item.visible = pagesDeleg.visible;
                }
            }

            function changeCommandsId(commandsId) {
                console.log("pagesDeleg changeCommandsId", topPanel.activeCommandsModelId , commandsId)
                topPanel.activeCommandsModelId = commandsId
            }

            function setModeMenuButton(commandId, mode) {
                console.log("ThumbnailDecorator setModeMenuButton!", commandId, mode);
                topPanel.setModeMenuButton(commandId, mode);
            }

            Loader {
                id: loader;
                anchors.fill: parent;

                Component.onCompleted: {
                    loader.source = "../imtlicgui/" + menuPanel.model.GetData(PageEnum.ID, model.index) + "MultiDocView.qml";
                }
                onItemChanged: {
                    if (loader.item){
                        loader.item.firstElementImageSource =  menuPanel.model.GetData(PageEnum.ICON, model.index);
                    }
                }

            }
        }
    }


//    ContactInfoEditor {
//        id: contactInfoEditor;
//        anchors.left: menuPanel.right;
//      //  width: tabPanel.width/4*3;
//        anchors.top: tabPanel.bottom;
//        anchors.bottom: parent.bottom;
//    }


//    TreeView {
//        id: treeView;
//        width: 200;
//        //height: parent.height - topPanel.height;
//        anchors.top: topPanel.bottom;
//        anchors.right: parent.right;
//    }

    Rectangle {
        id: darkBackground;
        anchors.fill: parent;
        color: "gray";
        opacity: 0.8;
        visible: preference.visible;
        z: 100;

        MouseArea {
            anchors.fill: parent;
        }
    }

    Preference {
        id: preference;
        visible: false;
        width: parent.width > 500 ? 500 : parent.width * 0.9;
        height: parent.height > 450 ? 450 : parent.height * 0.9;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;
        z: 110;
    }
}
