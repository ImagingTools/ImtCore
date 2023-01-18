import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: root;

    color: "gray";
    opacity: 0.5;

    Text {
        anchors.centerIn: parent;

        color: Style.textColor;

        font.pixelSize: Style.fontSize_title;
        font.family: Style.fontFamily;

        text: qsTr("Loading...");
    }

    MouseArea {
        anchors.fill: parent;
    }
}
