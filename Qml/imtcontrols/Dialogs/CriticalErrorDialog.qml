import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: messageDialog;

    width: 400;

    title: qsTr("Error");

    topPanelComp: Component {
        Item {}
    }

    property string message;

    contentComp: Component { MessageDialogBody {
            width: messageDialog.width;
            message: messageDialog.message;
        } }
}
