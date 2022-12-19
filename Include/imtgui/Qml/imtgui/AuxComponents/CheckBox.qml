import QtQuick 2.0
import Acf 1.0;

Rectangle {
    id: container;

    width: 13;
    height: width;

    color: "transparent";

    border.width: 1;
    border.color: isActive ? Style.borderColor : Style.disabledInActiveTextColor;

    property bool isActive: true;

    property int checkState: Qt.Unchecked;
    property alias imageSource: image.source;

    property string imageSourceActive: "../../../Icons/" + Style.theme + "/Ok_On_Normal.svg";
    property string imageSourceNotActive: "../../../Icons/" + Style.theme + "/Ok_On_Disabled.svg";

    signal clicked();

    Rectangle {
        id: rect;

        anchors.centerIn: parent;

        height: container.height - 4;
        width: height;

        color: Style.textColor;

        visible: container.checkState == Qt.PartiallyChecked;
    }

    Image {
        id: image;

        anchors.centerIn: parent;

        height: container.height - 1;
        width: height;

        sourceSize.width: width;
        sourceSize.height: height;

        visible: container.checkState != Qt.PartiallyChecked;
        source: container.isActive ? container.checkState == Qt.Checked ? container.imageSourceActive : "" :
            container.checkState == Qt.Checked ? container.imageSourceNotActive : "";
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
