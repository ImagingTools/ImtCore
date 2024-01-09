import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: inputDialogContainer;

    buttonsModel: ListModel{
        ListElement{Id: Enums.ButtonType.Ok; Name:qsTr("OK"); Enabled: true}
        ListElement{Id: Enums.ButtonType.Cancel; Name:qsTr("Cancel"); Enabled: true}
    }

    property string message;
    property string inputValue;

    Keys.onPressed: {
        if (event.key == Qt.Key_Enter){
            inputDialogContainer.finished(Enums.ButtonType.Ok);
        }
    }

    onFinished: {
        if (buttonId == Enums.ButtonType.Ok){
            inputDialogContainer.inputValue = inputDialogContainer.bodyItem.inputValue;
        }
    }

    onLocalizationChanged: {
        inputDialogContainer.buttonsModel.setProperty(0, "Name", qsTr("OK"));
        inputDialogContainer.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
    }

    contentComp: Component { InputDialogBody {
            width: inputDialogContainer.width;

            message: inputDialogContainer.message;
            inputValue: inputDialogContainer.inputValue;
        } }
}

