import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Dialog {
    id: inputDialogContainer;

    property string message;
    property string inputValue;

    Keys.onPressed: {
        if (event.key == Qt.Key_Enter){
            inputDialogContainer.finished("Ok");
        }
    }

    onFinished: {
        if (buttonId === "Ok"){
            inputDialogContainer.inputValue = inputDialogContainer.bodyItem.inputValue;
        }
    }

//    onMessageChanged: {
//        inputDialogContainer.bodyItem.message = inputDialogContainer.message;
//    }

//    onInputValueChanged: {
//        inputDialogContainer.bodyItem.inputValue = inputDialogContainer.inputValue;
//    }

    Component.onCompleted: {
        console.log("InputDialog onCompleted", inputDialogContainer);

        inputDialogContainer.buttons.addButton({"Id":"Ok", "Name":"OK", "Enabled": true, "Active": true});
        inputDialogContainer.buttons.addButton({"Id":"Cancel", "Name":"Cancel", "Enabled": true, "Active": false});

//        inputDialogContainer.bodySource = "InputDialogBody.qml";
        //inputDialogContainer.title = qsTr("Input");
    }

    contentComp: Component { InputDialogBody {
        message: inputDialogContainer.message;
        inputValue: inputDialogContainer.inputValue;
    } }
}

