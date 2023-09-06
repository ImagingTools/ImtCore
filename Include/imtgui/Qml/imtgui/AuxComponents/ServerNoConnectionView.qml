import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property string text: qsTr("There is no connection to the server. Please check server url.");

    Text {
        id: textNoConnection;

        anchors.centerIn: parent;

        width: parent.width - 100;

        text: container.text;

        horizontalAlignment: Text.AlignHCenter;

        color: Style.textColor;
        font.pixelSize: Style.fontSize_title;
        font.family: Style.fontFamily;

        wrapMode: Text.Wrap;
    }
}

