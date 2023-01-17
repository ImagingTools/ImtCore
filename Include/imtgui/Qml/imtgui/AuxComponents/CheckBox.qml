import QtQuick 2.0
import Acf 1.0;

Item {
    id: container;

    width: childrenRect.width;
    height: 13;

    property alias checkSize: checkRect.height;

    property bool isActive: true;

    property int checkState: Qt.Unchecked;
    property alias imageSource: image.source;

    property string imageSourceActive: "../../../Icons/" + Style.theme + "/Ok_On_Normal.svg";
    property string imageSourceNotActive: "../../../Icons/" + Style.theme + "/Ok_On_Disabled.svg";

    property alias text: checkBoxText.text;

    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;

    signal clicked();

    Rectangle {
        id: checkRect;
        anchors.verticalCenter: parent.verticalCenter;

        width: height;
        height: container.height;

        color: "transparent";

        border.width: 1;
        border.color: container.isActive ? Style.borderColor : Style.disabledInActiveTextColor;

        Image {
            id: image;

            anchors.centerIn: parent;

            height: checkRect.height - 1;
            width: height;

            sourceSize.width: width;
            sourceSize.height: height;

            visible: container.checkState != Qt.PartiallyChecked;
            source: container.isActive ? container.checkState == Qt.Checked ? container.imageSourceActive : "" :
                container.checkState == Qt.Checked ? container.imageSourceNotActive : "";
        }
    }

    Rectangle {
        id: rect;

        anchors.centerIn: parent;

        height: checkRect.height - 4;
        width: height;

        color: Style.textColor;

        visible: container.checkState == Qt.PartiallyChecked;
    }



    Text {
        id: checkBoxText;

        anchors.left: checkRect.right;
        anchors.leftMargin: 4;
        anchors.verticalCenter: container.verticalCenter;

        color: container.enabled ? Style.buttonText : Style.inactive_buttonText;

        font.pixelSize: container.fontPixelSize;
        font.family: Style.fontFamily;
        font.bold: container.fontBold;
    }

    MouseArea {
        anchors.fill: parent;

        cursorShape: Qt.PointingHandCursor;

        visible: container.isActive;

        onClicked: {
            container.clicked();
        }
    }
}
