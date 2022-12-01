import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Dialog {
    id: messageDialog;

    title: qsTr("Message");

    property string message;

    function addButton(){
        buttons.addButton();
    }

    onMessageChanged: {
        messageDialog.bodyItem.message = messageDialog.message;
    }

    Component.onCompleted: {
        messageDialog.buttons.addButton({"Id":"Yes", "Name":"Yes", "Enabled": true});
        messageDialog.buttons.addButton({"Id":"No", "Name":"No", "Enabled": true});

        messageDialog.bodySource = "MessageDialogBody.qml";
    }
}
