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
//    color: Style.backgroundColor;
//    color: "#00FF00";

//    property string fontName: mainFont.name;


    function updateModels() {
        console.log("updateModels");
        menuPanel.updateModels();
    }

//    Component.onCompleted: {
//        var obj = Object.defineProperty(preference, 'newProp',
//         {
//             enumerable: false,
//             configurable: false,
//             writable: true,
//             value: 'it is new property !!!'
//         })
//    }

//    FontLoader{
//        id: mainFont;
//        source: "../Fonts/Ubuntu-Regular.ttf";
//    }


    TopPanel {
        id: topPanel;
        title: menuPanel.activePageName;
        onMenuActivatedSignal: {
            console.log("onMenuActivatedSignal1",menuId, thubnailDecoratorContainer.activeItem)
            thubnailDecoratorContainer.activeItem.menuActivated(menuId);
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
            thubnailDecoratorContainer.activeItem.updateCommandId();
//            topPanel.activeCommandsModelId = menuPanel.activePageId

           // loader.source = "AuxComponents/" + menuPanel.activePageId + "View.qml"
        }
    }


    Repeater {
        id: pagesData;
//            anchors.fill: parent;
//        anchors.left: menuPanel.right;
//        anchors.right: parent.right;
//        anchors.top: topPanel.bottom;
//        anchors.bottom: parent.bottom;
        clip: true;
//            boundsBehavior: Flickable.StopAtBounds;
//            orientation: ListView.Horizontal;
//            spacing: 0;
//        model: menuPanel.pagesCount;
        model: menuPanel.model;

        delegate: Rectangle {
            id: pagesDeleg;
            anchors.left: menuPanel.right;
            anchors.right: thubnailDecoratorContainer.right;
            anchors.top: topPanel.bottom;
            anchors.bottom: thubnailDecoratorContainer.bottom;
//            width: thubnailDecoratorContainer.width - menuPanel.width;
//            height: thubnailDecoratorContainer.height - topPanel.height;
//            width: thubnailDecoratorContainer.width;
//            height: 500;
            color: "transparent";
            visible: menuPanel.activePageIndex === model.index;
            onVisibleChanged: {
                if(pagesDeleg.visible){
                    thubnailDecoratorContainer.activeItem = loader.item;
                }
            }

            function changeCommandsId(commandsId){
                console.log("pagesDeleg changeCommandsId", topPanel.activeCommandsModelId , commandsId)
                topPanel.activeCommandsModelId = commandsId
            }

            Loader {
                id: loader;
                anchors.fill: parent;
//                width: thubnailDecoratorContainer.width - menuPanel.width;
//                height: thubnailDecoratorContainer.height - topPanel.height;
                Component.onCompleted: {
                    loader.source = "../imtlicgui/" + menuPanel.model.GetData(PageEnum.ID, model.index) + "MultiDocView.qml";
//                    loader.source = "../imtlicgui/" + menuPanel.pagesSources[model.index] + "MultiDocView.qml";
//                    console.log("loader.source",loader.source, menuPanel.pagesSources)
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
        //textColor: Style.textColor;
        //color: Style.baseColor;
    }

//    Image {
//        width: 100;
//        height: 100;
////        source: Style.getImageSource("Axis", "Light", "On", "Active");
////        source: "../../Icons/Light/Axis_Off_Selected.svg";
//        sourceSize.height: 100;
//        sourceSize.width: 100;
//    }

//    EditLicenseDialog {
//        id: editLicenseDialog;

//        visible: true;
////        width: parent.width > 400 ? 400 : parent.width * 0.9;
////        height: parent.height > 300 ? 300 : parent.height * 0.9;

//        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.verticalCenter: parent.verticalCenter;
//    }
}
