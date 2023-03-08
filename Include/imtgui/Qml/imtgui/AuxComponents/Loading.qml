import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: root;

    color: Style.alternateBaseColor;

    Text {
        anchors.centerIn: parent;

        text: qsTr("Loading...");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }
}
