import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

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

    onLocalizationChanged: {
        inputDialogContainer.buttonsModel.setProperty(0, "Name", qsTr("OK"));
        inputDialogContainer.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
    }

    Component.onCompleted: {
        console.log("InputDialog onCompleted", inputDialogContainer);

        inputDialogContainer.buttons.addButton({"Id":"Ok", "Name":qsTr("OK"), "Enabled": true, "Active": true});
        inputDialogContainer.buttons.addButton({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true, "Active": false});
    }

    contentComp: Component { InputDialogBody {
        message: inputDialogContainer.message;
        inputValue: inputDialogContainer.inputValue;
    } }
}

