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
        let documentId = container.documentBase.documentModel.GetData("Id");
        if (commandId === "Close"){
            console.log("Close", documentId);
            console.log("container.documentBase.documentModel", container.documentBase.documentModel.toJSON());

            let itemId = container.documentBase.itemId;
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

        container.commandActivated(commandId);
    }
}
