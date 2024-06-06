import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: root;

    property Item errorPage: null;

    Component.onCompleted: {
        Events.subscribeEvent("SendCriticalError", root.showCriticalError);
        Events.subscribeEvent("SendWarningError", root.showWarningError);
        Events.subscribeEvent("SendError", root.onError);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SendCriticalError", root.showCriticalError);
        Events.unSubscribeEvent("SendWarningError", root.showWarningError);
        Events.unSubscribeEvent("SendError", root.onError);
    }

    function showCriticalError(message){
        return
    }

    function showWarningError(message){
        ModalDialogManager.openDialog(warningDialog, {"message": message});
    }

    function onError(parameters){
        let message = parameters["Message"];
        let errorType = parameters["ErrorType"];
        if (errorType === "Critical"){
            showCriticalError(message);
        }
        else if (errorType === "Warning"){
            showWarningError(message);
        }
        else{
        }
    }

    Component {
        id: warningDialog;
        ErrorDialog {
            title: qsTr("Warning message");
            onFinished: {}
        }
    }
}
