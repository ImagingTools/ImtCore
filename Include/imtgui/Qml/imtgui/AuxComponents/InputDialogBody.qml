import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: inputDialogBodyContainer;

    width: 300;
    height: columnBody.height;

    property string message;
    property string inputValue: "";

    Column {
        id: columnBody;

        width: inputDialogBodyContainer.width;

        spacing: 5;

        Text {
            id: message;

            width: columnBody.width;

            text: inputDialogBodyContainer.message;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            wrapMode: Text.WordWrap;
        }

        TextFieldCustom {
            id: tfcInput;

            width: columnBody.width - 20;
            height: 30;

            text: inputDialogBodyContainer.inputValue;
        }
    }
}

