import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'


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

    Component.onCompleted: {
        var obj = Object.defineProperty(preference, 'newProp',
         {
             enumerable: false,
             configurable: false,
             writable: true,
             value: 'it is new property !!!'
         })
    }

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
    }


    TabPanel {
        id:tabPanel;
        anchors.left: menuPanel.right;
        anchors.right: parent.right;
        anchors.top: topPanel.bottom;
        visible: true;
        //textColor: Style.textColor;
        //color: Style.baseColor;
        fontName: container.fontName;
    }

    AuxTable {
        id: table;
        anchors.left: menuPanel.right;
        width: tabPanel.width/4*3;
        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        fontName: container.fontName;
    }

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

    Image {
        width: 100;
        height: 100;
//        source: IconStyle.icon_Axis_On_Active;
        source: "../../Icons/Light/Axis_Off_Selected.svg";
        sourceSize.height: 100;
        sourceSize.width: 100;
    }
}
