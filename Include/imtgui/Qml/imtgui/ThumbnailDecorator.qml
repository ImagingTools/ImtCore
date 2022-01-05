import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
import imtauthgui 1.0
import imtlicgui 1.0

Rectangle
{
    id: container;
    anchors.fill: parent;
    width: 300;
    height: 200;
   // color: "white";
    color: Style.baseColor;
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
//        title: "Test";
        title: menuPanel.activePageName;
        activePageId: menuPanel.activePageId;
    }


//    Rectangle {
//        anchors.top: topPanel.bottom;
//        anchors.bottom: parent.bottom;
//        anchors.right: parent.right;
//        anchors.left: parent.left;
////        anchors.fill: parent;
//        color: "red";
//    }

    MenuPanel {
        id: menuPanel;

        anchors.top: topPanel.bottom;
        anchors.bottom: parent.bottom;
//        textColor: Style.textColor;
//        color: Style.backgroundColor;
//        fontName: container.fontName;
        onPagesCountChanged: {
            pagesData.model = menuPanel.pagesCount
        }

        onActivePageIdChanged: {
           // loader.source = "AuxComponents/" + menuPanel.activePageId + "View.qml"
        }
    }

//    PackagesView {

//        anchors.left: menuPanel.right;
//        anchors.right: parent.right;
//        anchors.top: topPanel.bottom;
//        anchors.bottom: parent.bottom;
//    }


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
            anchors.right: container.right;
            anchors.top: topPanel.bottom;
            anchors.bottom: container.bottom;
//            width: container.width - menuPanel.width;
//            height: container.height - topPanel.height;
            color: "transparent";
            visible: menuPanel.activePageIndex === model.index;
            Loader {
                id: loader;
                anchors.fill: parent;
//                width: container.width - menuPanel.width;
//                height: container.height - topPanel.height;
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
