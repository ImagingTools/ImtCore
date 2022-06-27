import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: tabPanelDecorator;

    width: texttabDelegate.width + imagetabDelegate.width + 30;



    Rectangle {
        anchors.fill: parent;
        color:  tabDelegate.selected ? Style.baseColor: "transparent";
    }

    Rectangle{
        id: selection;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: 2;

        color: Style.tabSelectedColor;
        visible: tabDelegate.selected;
    }

    Item {
        id: imagetabDelegate;

        height: parent.height;
        width: visible ? height : 1;

        visible: tabDelegate.firstElement;

        Image {
            id: firsElementImage;

            anchors.centerIn: parent;

            width: parent.width * 0.6;
            height: parent.height * 0.6;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
            source:  "../../../" + "Icons/" + Style.theme + "/" + tabDelegate.firstElementImageSource + "_On_Normal.svg";
        }
    }

    Item {
        id: texttabDelegate;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: imagetabDelegate.right;
        anchors.leftMargin: tabDelegate.firstElement ? 0 :10;

//        width: text.width + tabDelegate.height;
        width: text.width;

        onWidthChanged: {
            if (tabDelegate.autoWidth){
                tabDelegate.width = tabPanelDecorator.width;
            }
        }

        Text {
            id: text;

            anchors.centerIn: parent;

            text: tabDelegate.text;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            //elide: Text.ElideRight;
        }
    }
}

