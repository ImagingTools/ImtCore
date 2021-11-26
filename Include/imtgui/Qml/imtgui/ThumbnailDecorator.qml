import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
//@using { src.AuxComponents.TopPanel }
//@using { src.AuxComponents.MenuPanel }
//@using { src.AuxComponents.TabPanel }
//@using { src.AuxComponents.AuxTable }



Rectangle
{
    id: container;
    anchors.fill: parent;
    width: 300;
    height: 200;
    color: "white";
//    property string fontName: mainFont.name;


    function updateModels() {
        console.log("updateModels");
        menuPanel.updateModels();
    }

//    FontLoader{
//        id: mainFont;
//        source: "../Fonts/Ubuntu-Regular.ttf";
//    }

    TopPanel{
        id: topPanel;
//        title: "Test";
        title: menuPanel.activePageName;
        activePageId: menuPanel.activePageId;
//        fontName: container.fontName;
    }

    MenuPanel{
        id: menuPanel;
        anchors.top: topPanel.bottom;
        anchors.bottom: parent.bottom;
//        textColor: container.textColor;
//        fontName: container.fontName;
    }

    TabPanel{
        id:tabPanel;
        anchors.left: menuPanel.right;
        anchors.right: parent.right;
        anchors.top: topPanel.bottom;
        visible: true;
        textColor: container.textColor;
        fontName: container.fontName;
    }


    AuxTable {
        id: table;
        anchors.left: menuPanel.right;
        width: tabPanel.width/4*3;
        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        textColor: container.textColor;
        fontName: container.fontName;
    }

    TreeView {
        id: treeView;
        width: 200;
        height: parent.height - topPanel.height;
        anchors.top: topPanel.bottom;
        anchors.right: parent.right;
    }

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
    }
}
