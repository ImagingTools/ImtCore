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

    property bool blockUpdatingModel: false;

    // If true - after update the document will be closed
    property bool closingFlag: false;

    property bool nameOutsideEditor: false;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsDelegateSourceComp: commandsDelegateBase.sourceComponent;

    property CommandsProvider commandsProvider: CommandsProvider {
        commandsId: documentBase.commandsId;
    }

    signal commandsDelegateLoaded();

    property string documentUuid;
    UuidGenerator {
        id: uuidGenerator;
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Delete){
//            Events.sendEvent(documentBase.commandsId + "CommandActivated", "Remove");

            Events.sendEvent(documentBase.documentUuid + "CommandActivated", "Remove");
        }
    }

    Component.onCompleted: {
        documentBase.documentUuid = uuidGenerator.generateUUID();

        commandsDelegate.documentBase = documentBase;
    }

    onVisibleChanged: {
        if (visible){
            documentBase.commandsProvider.updateGui();
        }
    }

    onDocumentUuidChanged: {
        if (documentBase.documentUuid !== ""){
            documentBase.commandsProvider.documentUuid = documentBase.documentUuid;
            Events.subscribeEvent(documentBase.documentUuid + "CommandActivated", documentBase.commandsDelegate.commandHandle);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(documentBase.documentUuid + "CommandActivated", documentBase.commandsDelegate.commandHandle);

        documentBase.documentModel.modelChanged.disconnect(documentBase.modelChanged);
    }

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", documentBase.blockUpdatingModel);
    }

    onDocumentModelChanged: {
        console.log("documentBase onDocumentModelChanged", JSON.stringify(documentBase.documentModel));
        documentBase.documentModel.modelChanged.connect(documentBase.modelChanged);

        documentBase.itemId = documentBase.documentModel.GetData("Id");
        documentBase.itemName = documentBase.documentModel.GetData("Name");

        documentBase.updateDocumentTitle()
    }

    function onEntered(value){
        documentBase.documentModel.SetData("Id", value);
        documentBase.documentModel.SetData("Name", value);
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
        Events.sendEvent("DocumentIsDirtyChanged", {"Id": documentBase.itemId, "IsDirty": documentBase.isDirty});

        documentBase.commandsProvider.setCommandIsEnabled("Save", documentBase.isDirty);
    }

    onDocumentsDataChanged: {
        if(commandsDelegateBase.item && commandsDelegateBase.item.documentsData !==undefined){
            commandsDelegateBase.item.documentsData = documentBase.documentsData;
        }
    }

    onDocumentManagerChanged: {
        if(commandsDelegateBase.item){
            commandsDelegateBase.item.documentBase = documentBase;
        }
    }

    onCommandsIdChanged: {
        console.log("documentBase onCommandsIdChanged", documentBase.commandsId, itemName);
        commandsDelegate.commandsId = documentBase.commandsId;
    }

    function updateDocumentTitle(){
        if (documentBase.documentManager != null){
            let documentId = documentBase.documentModel.GetData("Id");
            let documentName = documentBase.documentModel.GetData("Name");
            if (documentName === ""){
                documentName = "<new item>"
            }

            documentBase.documentManager.setDocumentTitle({"Id": documentId, "Title": documentName});
        }
    }

    function updateGui(){}

    function updateModel(){}

    Shortcut {
        sequence: "Ctrl+S";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+S onActivated");
            Events.sendEvent(documentBase.documentUuid + "CommandActivated", "Save");
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+Z onActivated");
            Events.sendEvent(documentBase.documentUuid + "CommandActivated", "Undo");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";

        enabled: documentBase.visible;

        onActivated: {
            console.log("Ctrl+Shift+Z onActivated");
            Events.sendEvent(documentBase.documentUuid + "CommandActivated", "Redo");
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

//    function close(){
//        Events.sendEvent(documentBase.commandsId + "CommandActivated", "Close");
//    }

//    function save(){
//        Events.sendEvent(documentBase.commandsId + "CommandActivated", "Save");
//    }
}
