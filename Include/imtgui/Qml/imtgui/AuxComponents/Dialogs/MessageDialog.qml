import QtQuick 2.12
import Acf 1.0
import imtqml 1.0


Dialog {
    id: messageDialog;

    property string message;

    onMessageChanged: {
        messageDialog.bodyItem.message = messageDialog.message;
    }

    Component.onCompleted: {
        messageDialog.buttons.addButton({"Id":"Yes", "Name":"Yes"});
        messageDialog.buttons.addButton({"Id":"No", "Name":"No"});
        messageDialog.bodySource = "MessageDialogBody.qml";
        messageDialog.title = qsTr("Message");
    }
}
