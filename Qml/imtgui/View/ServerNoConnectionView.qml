import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: container;

    color: Style.backgroundColor;

    property alias text: textNoConnection.text;
    property alias loadingVisible: loading.visible;

    Column {
        id: content;

        anchors.centerIn: parent;

        width: parent.width - 100;

        spacing: 10;

        Text {
            id: textNoConnection;

            anchors.horizontalCenter: parent.horizontalCenter;

            horizontalAlignment: Text.AlignHCenter;

            color: Style.textColor;
            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            wrapMode: Text.Wrap;
        }

        Loading {
            id: loading;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: 100;
            height: 100;

            visible: false;
        }
    }
}
