import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: collectionMetaInfo;

    anchors.right: parent.right;

    height: parent.height;
    width: 200;

    color: Style.backgroundColor;

    property string modTimeTitle: "Modification Time";
    property string dataTitle: "Features";
    property string checkSumTitle: "Checksum";
    property string modTimeValue;
    property string checkSumValue;
    property var modelData;

    Text {
        id: dataTitle;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        font.pixelSize: Style.fontSize_small;
        font.family: Style.fontFamilyBold;
        font.bold: true;
        color: Style.textColor;
        text: collectionMetaInfo.dataTitle;
    }

    Column {
        id: metaInfoData;

        anchors.top: dataTitle.bottom;
        anchors.topMargin: 5;

        width: parent.width;

        Repeater {
            model: collectionMetaInfo.modelData;
            delegate: Rectangle {
                id: repeaterDelegate;

                width: parent.width;
                height: 20;

                color: "transparent";

                Text {
                    id: textData;

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    text: model.Name;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_small;
                    color: Style.textColor;
                }
            }
        }
    }

    Text {
        id: modTimeTitle;
        anchors.top: metaInfoData.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        font.pixelSize: Style.fontSize_small;
        font.family: Style.fontFamilyBold;
        font.bold: true;

        color: Style.textColor;
        text: collectionMetaInfo.modTimeTitle;
    }

    Text {
        id: modTime;

        anchors.top: modTimeTitle.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_small;
        color: Style.textColor;
        text: collectionMetaInfo.modTimeValue;
    }

    Text {
        id: checkSumTitle;
        anchors.top: modTime.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        font.pixelSize: Style.fontSize_small;
        font.family: Style.fontFamilyBold;
        font.bold: true;
        color: Style.textColor;
        text: collectionMetaInfo.checkSumTitle;
    }

    Text {
        id: checksumValue;

        anchors.top: checkSumTitle.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_small;
        color: Style.textColor;
        text: collectionMetaInfo.checkSumValue;
    }
}
