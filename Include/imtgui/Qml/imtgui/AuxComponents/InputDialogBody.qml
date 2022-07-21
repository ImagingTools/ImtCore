import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Item {
    id: inputDialogBodyContainer;

//    width: 300;
    height: columnBody.height + 40;

    onHeightChanged: {
        //loaderBodyDialog.itemHeightChanged();
    }

    property string message;
    property string inputValue: "";

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 10;

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

        TextFieldCustom {
            id: tfcInput;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: columnBody.width;
            height: 30;

            text: inputDialogBodyContainer.inputValue;
        }
    }
}

