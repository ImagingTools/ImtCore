import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

SingleDocumentWorkspaceView {
    id: container;
    Component.onCompleted: {
    startPageObj = {"Id": "Administration",
        "Name": "Administration",
        "Source": "../../imtauthgui/AdministrationView.qml",
        "CommandsId": "Administration"};    
    }

}

