import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0


Row {
    id: content;

    spacing: Style.size_mainMargin;

    property alias icon: image_
    property alias text: infoText.text
    property alias checked: switchItem.checked

    Image {
        id: image_;

        anchors.verticalCenter: content.verticalCenter;

        width: visible ? Style.itemSizeMedium * 0.8 : 0;
        height: width;

        sourceSize.width: width;
        sourceSize.height: height;
    }

    RadioButton {
        id: switchItem

        anchors.verticalCenter: content.verticalCenter;

        autoExclusive: false;
        checked: true
    }

    Text {
        id: infoText;

        anchors.verticalCenter: content.verticalCenter;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }



}



