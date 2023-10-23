import QtQuick 2.12
import Acf 1.0

QtObject {
    id: container;

    property Item documentBase: null;

    property bool showInputIdDialog: false;

    signal entered(string value);
    signal saved(string id, string name);
    signal closed();
    signal commandActivated(string commandId);

    function commandHandle(commandId){
        console.log("commandHandle", commandId);
        let documentId = "";
        let documentModel = container.documentBase.documentModel;
        if (documentModel.ContainsKey("Id")){
            documentId = documentModel.GetData("Id");
        }

        console.log("documentId", documentId);

        if (commandId === "Close"){
            let itemId = container.documentBase.documentUuid;
            let documentManager = container.documentBase.documentManager;
            documentManager.closeDocument(itemId);
        }
        else if (commandId === "Save"){
            if (container.documentBase.isDirty){
                let itemId = container.documentBase.itemId;
                let documentManager = container.documentBase.documentManager;
                documentManager.saveDocument(itemId);
            }
        }
        else if (commandId === "History"){
            let itemId = container.documentBase.itemId;

            modalDialogManager.openDialog(documentHistoryDialogComp, {"documentId": itemId, "documentTypeId": container.documentBase.commandsId});
        }

        container.commandActivated(commandId);
    }

    property Component documentHistoryDialogComp: Component {
        DocumentHistoryDialog {
            title: qsTr("Document history");

            onStarted: {
                updateModel();
            }
        }
    }
}


