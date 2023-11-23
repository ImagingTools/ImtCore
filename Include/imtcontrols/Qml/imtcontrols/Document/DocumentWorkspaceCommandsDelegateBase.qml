import QtQuick 2.12
import Acf 1.0

QtObject {
    id: container;

    property Item documentPtr: null;

    signal commandActivated(string commandId);

    function commandHandle(commandId){
        console.log("commandHandle", commandId);

        if (commandId === "Close"){
            onClose();
        }
        else if (commandId === "Save"){
            onSave();
        }
        else{
            container.commandActivated(commandId);
        }
    }

    function onClose(){
        if (documentPtr && documentPtr.documentManagerPtr){
            documentPtr.documentManagerPtr.closeDocument(documentPtr.uuid);
        }
    }

    function onSave(){
        if (documentPtr && documentPtr.documentManagerPtr){
            documentPtr.documentManagerPtr.saveDocument(documentPtr.uuid);
        }
    }
}
