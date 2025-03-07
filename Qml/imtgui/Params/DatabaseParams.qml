import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcontrols 1.0

Column {
    id: root;
    width: 700;
    spacing: Style.sizeLargeMargin;

    property var parameters: model.Parameters

    GroupHeaderView {
        id: headerView;
        width: parent.width;
        title: qsTr("Database Information");
    }

    DatabaseAccessSettingsEditor {
        id: bodyPanel;
        width: parent.width;
        model: root.parameters;

        Component.onCompleted: {
            hostInput.readOnly = true;
            usernameInput.readOnly = true;
        }
    }

    ButtonElementView {
        id: buttonElementView;
        width: parent.width;
        name: qsTr("Test database connection");
        text: qsTr("Test");
        onClicked: {
            let env = "PGPASSWORD=" + bodyPanel.passwordInput.text;
            process.setEnviroment([env])
            process.start("psql", ["-U", "postgres", "-d", bodyPanel.databaseNameInput.text, "-c", "SELECT 1"]);
        }
    }

    DatabaseBackupController {
        id:databaseBackupController;
        width: parent.width;
    }

    // Component in CPP from imtqml
    Process {
        id: process;
        onFinished: {
            buttonElementView.bottomComp = exitCode != 0 ? errorComp : connectionComp;
        }
    }

    Component {
        id: errorComp;
        BaseText {
            text: qsTr("There is no connection to the database");
            color: Style.errorTextColor;
        }
    }

    Component {
        id: connectionComp;
        BaseText {
            text: qsTr("Test connection successfully");
            color: Style.greenColor;
        }
    }
}

