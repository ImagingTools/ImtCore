import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: tabPanelDecorator;

    width: texttabDelegate.width + imagetabDelegate.width + 30;
    property Item rootItem: null;


    Rectangle {
        anchors.fill: parent;
        //color:  tabDelegate.selected ? Style.baseColor: "transparent";
        color: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.selected ? Style.baseColor: "transparent": "transparent";
    }

    Rectangle{
        id: selection;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: 2;

        color: Style.tabSelectedColor;
        //visible: tabDelegate.selected;
        visible: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.selected: false;

    }

    Item {
        id: imagetabDelegate;

        height: parent.height;
        width: visible ? height : 1;

        //visible: tabDelegate.firstElement;
        visible: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.firstElement: false;

        Image {
            id: firsElementImage;

            anchors.centerIn: parent;

            width: parent.width * 0.6;
            height: parent.height * 0.6;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
            //source:  "../../../Icons/" + Style.theme + "/" + tabDelegate.firstElementImageSource + "_On_Normal.svg";
            source: tabPanelDecorator.rootItem ?  "../../../Icons/" + Style.theme + "/" + tabPanelDecorator.rootItem.firstElementImageSource + "_On_Normal.svg": "";
        }
    }

    Item {
        id: texttabDelegate;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: imagetabDelegate.right;
        //anchors.leftMargin: tabDelegate.firstElement ? 0 :10;
        anchors.leftMargin: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.firstElement ? 0 :10 : 0;

//        width: text.width + tabDelegate.height;
        width: text.width;

        onWidthChanged: {
//            if (tabDelegate.autoWidth){
//                tabDelegate.width = tabPanelDecorator.width;
//            }
            if (tabPanelDecorator.rootItem && tabPanelDecorator.rootItem.autoWidth){
                tabPanelDecorator.rootItem.width = tabPanelDecorator.width;
            }
        }

        Text {
            id: text;

            anchors.centerIn: parent;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            //elide: Text.ElideRight;
            //text: tabDelegate.text;
            text: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.text : "";
        }
    }
}

