import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: messageDialog;

    title: qsTr("Message");

    property string message;
    property int contentWidth: 300;

    canMove: true;

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.yes, Name:qsTr("Yes"), Enabled: true})
        buttonsModel.append({Id: Enums.no, Name:qsTr("No"), Enabled: true})
    }

    Keys.onPressed: {
        console.log("MessageDialog onPressed", event.key);

        if (event.key == Qt.Key_Return){
            messageDialog.finished(Enums.yes);
        }
    }

    onLocalizationChanged: {
        messageDialog.buttonsModel.setProperty(0, "Name", qsTr("Yes"));
        messageDialog.buttonsModel.setProperty(1, "Name", qsTr("No"));
    }

    contentComp: Component {
        MessageDialogBody {
            width: messageDialog.width;
            message: messageDialog.message;
        }
    }
}
