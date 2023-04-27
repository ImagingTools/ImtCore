import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property Item documentBase: null;

    property bool showInputIdDialog: false;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    property int updateItemTimer;

    signal entered(string value);
    signal saved(string id, string name);
    signal closed();
    signal commandActivated(string commandId);
    signal itemLoaded();

    property var itemModelInputParams: ({});

    function commandHandle(commandId){
        console.log("DocumentCommandsBase commandHandle", container.documentBase.itemId, commandId);

        let documentId = container.documentBase.documentModel.GetData("Id");

        if (commandId === "Close"){
            console.log("Close");
            let itemId = container.documentBase.itemId;
            let documentManager = container.documentBase.documentManager;
            documentManager.closeDocument(itemId);
        }
        else if (commandId === "Save"){
            console.log("Save");
            console.log("container.documentBase.isDirty", container.documentBase.isDirty);
            if (container.documentBase.isDirty){
                let itemId = container.documentBase.itemId;
                let documentManager = container.documentBase.documentManager;
                documentManager.saveDocument(itemId);
            }
        }

        container.commandActivated(commandId);
    }
}
