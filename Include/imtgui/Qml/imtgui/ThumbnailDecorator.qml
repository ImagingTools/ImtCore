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
        title: menuPanel.activePageName;
        activePageId: menuPanel.activePageId;
//        fontName: container.fontName;
    }

    MenuPanel{
        id: menuPanel;
        anchors.top: topPanel.bottom;
        anchors.bottom: parent.bottom;
        textColor: container.textColor;
        fontName: container.fontName;
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


}
