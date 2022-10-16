import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {

    Component {
        id: loginDialog;

        LoginDialog {

        }
    }

    AuxButton {
        id: loginButton;
        anchors.verticalCenter: parent.verticalCenter;

        width: 24;
        height: 24;

        iconSource: "../../../Icons/" + Style.theme + "/Account.svg";

        onClicked: {
            var source = "AuxComponents/Dialogs/LoginDialog.qml";
            var parameters = {};

            modalDialogManager.openDialog(loginDialog, parameters);
        }
    }
}

