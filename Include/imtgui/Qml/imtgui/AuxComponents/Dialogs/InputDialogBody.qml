import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: inputDialogBodyContainer;

    height: columnBody.height + 40;

    property string message;
    property string inputValue: "";
    property Item rootItem: null;

    onFocusChanged: {
        console.log("InputBody onFocusChanged", focus);

        if (inputDialogBodyContainer.focus){
            inputField.focus = inputDialogBodyContainer.focus;
        }
    }

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 10;

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

        CustomTextField {
            id: inputField;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: columnBody.width;
            height: 30;

            text: inputDialogBodyContainer.inputValue;

            onTextChanged: {
                inputDialogBodyContainer.inputValue = inputField.text;
            }

            onAccepted: {
                inputDialogBodyContainer.rootItem.buttons.buttonClicked("Ok");
                //buttonsDialog.buttonClicked("Ok");
            }
        }
    }
}

