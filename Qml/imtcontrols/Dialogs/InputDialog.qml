import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: inputDialogContainer;

    property string message;
    property string inputValue;
    property int contentWidth: 300;

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.ok, Name:qsTr("OK"), Enabled: true})
        buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Enter){
            inputDialogContainer.finished(Enums.ok);
        }
    }

    onFinished: {
        if (buttonId == Enums.ok){
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

