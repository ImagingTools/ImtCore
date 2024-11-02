import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Item {
    id: userPanelDelegate;

    property UserData userModel: UserData {}

    signal userUpdated();

    property var dialogManager: ModalDialogManager;

    function logout(){
        Events.sendEvent("Logout");
    }
}

