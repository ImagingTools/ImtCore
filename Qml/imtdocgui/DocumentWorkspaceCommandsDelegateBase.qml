import QtQuick 2.12
import Acf 1.0

QtObject {
    id: container;

    property Item documentPtr: null;

    property Component documentHistoryDialogComp: Component {
        DocumentHistoryDialog {
            title: qsTr("Document history");

            onStarted: {
                updateModel();
            }
        }
    }

    signal commandActivated(string commandId);

    function commandHandle(commandId){
        console.log("commandHandle", commandId);

        if (commandId === "Close"){
            onClose();
        }
        else if (commandId === "Save"){
            onSave();
        }
        else if (commandId === "History"){
            onHistory();
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
            documentPtr.documentManagerPtr.saveDocument(documentPtr.uuid, documentPtr.getAdditionalInputParams());
        }
    }

    function onHistory(){
        if (documentPtr){
            let documentId = documentPtr.documentId;
            let documentTypeId = documentPtr.documentTypeId;

            modalDialogManager.openDialog(documentHistoryDialogComp, {"documentId": documentId, "documentTypeId": documentTypeId});
        }
    }
}
