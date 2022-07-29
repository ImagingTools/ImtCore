import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Dialog {
    id: inputDialogContainer;

    property string message;
    property string startValue;

    onMessageChanged: {
        inputDialogContainer.bodyItem.message = message;
    }

    onStartValueChanged: {
        inputDialogContainer.bodyItem.inputValue = startValue;
    }

    Component.onCompleted: {
        console.log("InputDialog onCompleted", inputDialogContainer);

        inputDialogContainer.buttons.addButton({"Id":"Ok", "Name":"OK"});
        inputDialogContainer.buttons.addButton({"Id":"Cancel", "Name":"Cancel"});

        inputDialogContainer.bodySource = "InputDialogBody.qml";
        inputDialogContainer.title = qsTr("Input");
    }
}

