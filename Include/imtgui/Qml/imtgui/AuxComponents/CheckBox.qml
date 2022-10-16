import QtQuick 2.0
import Acf 1.0;

Rectangle {
    id: container;

    width: 13;
    height: width;

    color: "transparent";

    border.width: 1;
    border.color: Style.borderColor;

    property int checkState: Qt.Unchecked;

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
        anchors.centerIn: parent;

        height: container.height - 1;
        width: height;

        sourceSize.width: width;
        sourceSize.height: height;

        visible: container.checkState != Qt.PartiallyChecked;
        source: container.checkState == Qt.Checked ? "../../../Icons/" + Style.theme + "/Ok_On_Normal.svg" : "";
    }

    MouseArea {
        anchors.fill: parent;

        cursorShape: Qt.PointingHandCursor;

        onClicked: {
            container.clicked();
        }
    }
}
