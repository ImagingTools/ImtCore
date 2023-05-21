import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: documentBaseRoot;

    property string itemId;
    property string itemName;
    property string commandsId;
    property string documentUuid;

    property TreeItemModel documentModel: TreeItemModel {};
    property TreeItemModel documentsData: TreeItemModel{};
    property Item documentManager: null;

    property bool isDirty: false;
    property bool readOnly: false;

    property bool saveIsBlocked: false;

    property bool blockUpdatingModel: false;

    // If true - after update the document will be closed
    property bool closingFlag: false;

    property bool modelIsReady: false;

    property bool nameOutsideEditor: false;

    property alias commandsDelegate: commandsDelegateBase.item;
    property alias commandsDelegateSourceComp: commandsDelegateBase.sourceComponent;

    property CommandsProvider commandsProvider: CommandsProvider {
        commandsId: documentBaseRoot.commandsId;
    }

    signal commandsDelegateLoaded();

    signal saved();

    UuidGenerator {
        id: uuidGenerator;
    }

    function documentCanBeSaved(){
        return true;
    }

    function blockEditing(){}

    Component.onCompleted: {
        console.log("Document onCompleted", itemId);
        documentBaseRoot.documentUuid = uuidGenerator.generateUUID();

        documentBaseRoot.commandsProvider.modelLoaded.connect(documentBaseRoot.onCommandsModelLoaded);
        documentBaseRoot.commandsDelegate.documentBase = documentBaseRoot;
    }

    onVisibleChanged: {
        if (visible){
            documentBaseRoot.commandsProvider.updateGui();
        }
    }

    onDocumentUuidChanged: {
        if (documentBaseRoot.documentUuid !== ""){
            documentBaseRoot.commandsProvider.documentUuid = documentBaseRoot.documentUuid;
            Events.subscribeEvent(documentBaseRoot.documentUuid + "CommandActivated", documentBaseRoot.commandsDelegate.commandHandle);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(documentBaseRoot.documentUuid + "CommandActivated", documentBaseRoot.commandsDelegate.commandHandle);

        documentBaseRoot.commandsProvider.modelLoaded.disconnect(documentBaseRoot.onCommandsModelLoaded);
    }

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", documentBaseRoot.blockUpdatingModel);
    }

    onDocumentModelChanged: {
        console.log("onDocumentModelChanged", documentBaseRoot.documentModel);

        documentBaseRoot.documentModel.dataChanged.connect(documentBaseRoot.onDataChanged);

        documentBaseRoot.itemId = documentBaseRoot.documentModel.GetData("Id");
        documentBaseRoot.itemName = documentBaseRoot.documentModel.GetData("Name");

        documentBaseRoot.updateDocumentTitle()

        documentBaseRoot.modelIsReady = true;
    }

    function onDataChanged(topLeft, bottomRight, roles){
        documentBaseRoot.modelChanged();
    }

    onIsDirtyChanged: {
        Events.sendEvent("DocumentIsDirtyChanged", {"Id": documentBaseRoot.itemId, "IsDirty": documentBaseRoot.isDirty});

        documentBaseRoot.commandsProvider.setCommandIsEnabled("Save", documentBaseRoot.isDirty);
    }

    onCommandsIdChanged: {
        if (documentBaseRoot.itemId === ""){
            documentBaseRoot.documentModel.dataChanged.connect(documentBaseRoot.onDataChanged);

            documentBaseRoot.updateModel();
            documentBaseRoot.modelIsReady = true;
        }
    }

    function onEntered(value){
        documentBaseRoot.documentModel.SetData("Id", value);
        documentBaseRoot.documentModel.SetData("Name", value);
    }

    function modelChanged(){
        console.log("DocumentsCommands modelChanged");
        if (documentBaseRoot.blockUpdatingModel){
            return;
        }

        let saveExists = documentBaseRoot.commandsProvider.commandExists("Save");
        if (!saveExists){
            return;
        }

        if (!documentBaseRoot.isDirty){
            documentBaseRoot.isDirty = true;
        }
    }

    function updateDocumentTitle(){
        if (documentBaseRoot.documentManager != null){
            let documentId = documentBaseRoot.documentModel.GetData("Id");
            let documentName = documentBaseRoot.documentModel.GetData("Name");
            if (documentName === ""){
                documentName = "<new item>"
            }

            documentBaseRoot.documentManager.setDocumentTitle({"Id": documentId, "Title": documentName});
        }
    }

    // If the command 'Save' is missing then we block the UI
    function onCommandsModelLoaded(){
        let saveExists = documentBaseRoot.commandsProvider.commandExists("Save");
        if (!saveExists){
            documentBaseRoot.blockEditing();
        }
    }

    function updateGui(){}

    function updateModel(){}

    Shortcut {
        sequence: "Ctrl+S";

        enabled: documentBaseRoot.visible;

        onActivated: {
            console.log("Ctrl+S onActivated");
            Events.sendEvent(documentBaseRoot.documentUuid + "CommandActivated", "Save");
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";

        enabled: documentBaseRoot.visible;

        onActivated: {
            console.log("Ctrl+Z onActivated");
            Events.sendEvent(documentBaseRoot.documentUuid + "CommandActivated", "Undo");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";

        enabled: documentBaseRoot.visible;

        onActivated: {
            console.log("Ctrl+Shift+Z onActivated");
            Events.sendEvent(documentBaseRoot.documentUuid + "CommandActivated", "Redo");
        }
    }

    Loader {
        id: commandsDelegateBase;

        sourceComponent: DocumentWorkspaceCommandsDelegateBase {
        }

        onLoaded: {
            commandsDelegateBase.item.documentBase = documentBaseRoot;

            documentBaseRoot.commandsDelegateLoaded();
        }
    }
}
