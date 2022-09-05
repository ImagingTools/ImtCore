import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Dialog {
    id: inputDialogContainer;

    property string message;
    property string inputValue;

    onFinished: {
        if (buttonId === "Ok"){
            inputDialogContainer.inputValue = inputDialogContainer.bodyItem.inputValue;
        }
    }

    onMessageChanged: {
        inputDialogContainer.bodyItem.message = message;
    }

    onInputValueChanged: {
        inputDialogContainer.bodyItem.inputValue = inputValue;
    }

    Component.onCompleted: {
        console.log("InputDialog onCompleted", inputDialogContainer);

        inputDialogContainer.buttons.addButton({"Id":"Ok", "Name":"OK", "Enabled": true, "Active": true});
        inputDialogContainer.buttons.addButton({"Id":"Cancel", "Name":"Cancel", "Enabled": true, "Active": false});

        inputDialogContainer.bodySource = "InputDialogBody.qml";
        inputDialogContainer.title = qsTr("Input");
    }
}

