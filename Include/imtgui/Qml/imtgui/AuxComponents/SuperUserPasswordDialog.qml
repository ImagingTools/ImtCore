import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Dialog {
    id: inputDialogContainer;

    property string message;
    property string inputValue;

    onFinished: {
        if (buttonId === "Ok"){
            inputDialogContainer.inputValue = contentItem.inputValue;
        }
    }

    contentComp: Item {
        id: inputDialogBodyContainer;

        height: columnBody.height + 40;

//        property string message;
        property alias inputValue: inputField.text;


        onFocusChanged: {
            if (focus){
                inputField.focus = focus;
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

                text: inputDialogContainer.message;
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

                text: inputDialogContainer.inputValue;

                echoMode: TextInput.Password;

                onTextChanged: {
                    let state = text != "";
//                    inputDialogContainer.buttons.setButtonState("Ok", state)
                }

                onAccepted: {
                    inputDialogContainer.buttons.buttonClicked("Ok");
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("InputDialog onCompleted", inputDialogContainer);

        inputDialogContainer.buttons.addButton({"Id":"Ok", "Name":"OK", "Enabled": false, "Active": true});
    }
}
