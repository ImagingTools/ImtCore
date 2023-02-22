import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: documentBase;

//    anchors.fill: parent;

    property string itemId;
    property string itemName;
    property string commandsId;

    property TreeItemModel documentModel: TreeItemModel {};
    property TreeItemModel documentsData: TreeItemModel{};
    property Item documentManager: null;

    // If you need to request a model from the server then TRUE
    property bool itemLoad: true;

    property bool isDirty: false;

    property bool saveIsBlocked: false;

    // If true - after update the document will be closed
    property bool closingFlag: false;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsDelegateSourceComp: commandsDelegateBase.sourceComponent;

    property CommandsProvider commandsProvider: CommandsProvider {
        commandsId: documentBase.commandsId;
    }

    signal commandsDelegateLoaded();

    Keys.onPressed: {
        if (event.key == Qt.Key_Delete){
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Remove");
        }
    }

    Component.onCompleted: {
        commandsDelegate.documentBase = documentBase;
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(documentBase.commandsId + "CommandActivated", documentBase.commandsDelegate.commandHandle);
        documentBase.documentModel.modelChanged.disconnect(documentBase.modelChanged);
    }

    onDocumentModelChanged: {
        documentBase.documentModel.modelChanged.connect(documentBase.modelChanged);
    }

    function modelChanged(){
        console.log("DocumentsCommands modelChanged");

        let saveExists = documentBase.commandsProvider.commandExists("Save");
        if (!saveExists){
            return;
        }

        if (!documentBase.isDirty){
            documentBase.isDirty = true;
        }
    }

    onIsDirtyChanged: {
        Events.sendEvent("DocumentIsDirtyChanged", documentBase.isDirty);

        documentBase.commandsProvider.setCommandIsEnabled("Save", documentBase.isDirty);
    }

    onDocumentsDataChanged: {
        if(commandsDelegateBase.item && commandsDelegateBase.item.documentsData !==undefined){
            commandsDelegateBase.item.documentsData = documentBase.documentsData;
        }
//        if (documentBase.itemLoad){
//            documentBase.itemId = documentsData.GetData("Id", model.index);
//            documentBase.itemName = documentsData.GetData("Title", model.index);
//        }
    }

    onDocumentManagerChanged: {
        if(commandsDelegateBase.item){
            commandsDelegateBase.item.documentBase = documentBase;
        }
    }

    onCommandsIdChanged: {
        console.log("documentBase onCommandsIdChanged", documentBase.commandsId, itemId);

        if (documentBase.itemLoad){
            Events.subscribeEvent(documentBase.commandsId + "CommandActivated", documentBase.commandsDelegate.commandHandle);

            commandsDelegate.commandsId = documentBase.commandsId;

            documentBase.commandsDelegate.itemModelInputParams["Id"] = itemId;
            documentBase.commandsDelegate.updateModel();
        }
    }

//    onVisibleChanged: {
//        if (documentBase.visible){
//            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
//                                 "CommandsId": commandsProvider.commandsId});

//            Events.subscribeEvent(documentBase.commandsId + "CommandActivated", documentBase.commandsDelegate.commandHandle);
//        }
//        else{
//            Events.unSubscribeEvent(documentBase.commandsId + "CommandActivated", documentBase.commandsDelegate.commandHandle);
//        }
//    }

    function updateGui(){}

    function updateModel(){}

    Shortcut {
        sequence: "Ctrl+S";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+S onActivated");
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Save");
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+Z onActivated");
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Undo");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+Shift+Z onActivated");
            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Redo");
        }
    }

    Loader {
        id: commandsDelegateBase;

        sourceComponent: DocumentWorkspaceCommandsDelegateBase {
        }

        onLoaded: {
            commandsDelegateBase.item.documentBase = documentBase;

            if(commandsDelegateBase.item.documentsData !==undefined){
                commandsDelegateBase.item.documentsData = documentBase.documentsData;
            }

            documentBase.commandsDelegateLoaded();
        }
    }

    function close(){
        Events.sendEvent(documentBase.commandsId + "CommandActivated", "Close");
    }

    function save(){
        Events.sendEvent(documentBase.commandsId + "CommandActivated", "Save");
    }
}
