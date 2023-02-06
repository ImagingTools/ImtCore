import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: inputDialogBodyContainer;

    height: Style.size_minMessageHeight == undefined ? columnBody.height + 40 :
                                                      Math.max(Style.size_minMessageHeight, columnBody.height + 40);
    color: "transparent";

    property string message;

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 20;
        anchors.leftMargin: 20;

        width: inputDialogBodyContainer.width;

        spacing: 5;

        Text {
            id: messageText;

            width: columnBody.width;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            elide: Text.ElideRight;
            wrapMode: Text.WordWrap;

            text: inputDialogBodyContainer.message;
        }
    }
}

