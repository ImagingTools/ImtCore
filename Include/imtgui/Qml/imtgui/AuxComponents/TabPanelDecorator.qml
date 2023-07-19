import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: tabPanelDecorator;

    width: texttabDelegate.width + imagetabDelegate.width + 40;
    property Item rootItem: null;

    property string firstElementImageSource: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.firstElementImageSource : "";
    onFirstElementImageSourceChanged: {
        if (tabPanelDecorator.firstElementImageSource !== ""){
            firsElementImage.source = "../../../Icons/" + Style.theme + "/" + tabPanelDecorator.firstElementImageSource + "_On_Normal.svg";
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.selected ? Style.baseColor: "transparent": "transparent";
    }

    Rectangle{
        id: selection;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: 2;

        color: Style.tabSelectedColor;
        visible: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.selected: false;
    }

    Item {
        id: imagetabDelegate;

        height: parent.height;
        width: visible ? height : 1;

        visible: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.firstElement: false;

        Image {
            id: firsElementImage;

            anchors.centerIn: parent;

            width: parent.width * 0.6;
            height: parent.height * 0.6;

            sourceSize.width: width;
            sourceSize.height: height;

            fillMode: Image.PreserveAspectFit;
        }
    }

    Item {
        id: texttabDelegate;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: imagetabDelegate.right;
        anchors.leftMargin: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.firstElement ? 0 :10 : 0;

        width: text.width;

        Text {
            id: text;

            anchors.centerIn: parent;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            text: tabPanelDecorator.rootItem ? tabPanelDecorator.rootItem.text : "";
        }
    }
}

