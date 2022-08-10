import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: inputDialogBodyContainer;

    height: columnBody.height + 40;

    property string message;

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 20;

        width: inputDialogBodyContainer.width;

        spacing: 5;

        Text {
            id: message;

            width: columnBody.width;

            text: inputDialogBodyContainer.message;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            elide: Text.ElideRight;

            wrapMode: Text.WordWrap;
        }
    }
}

