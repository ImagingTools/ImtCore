import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0


Item {
    id: documentData;

    property string uuid;
    property string documentId;

    property string documentTypeId;
    property bool isDirty: false;
    property bool readOnly: false;
    property TreeItemModel documentModel: TreeItemModel {};
    property UndoRedoManager undoManagerPtr: null;

    property var documentManagerPtr: null;

    property CommandsProvider commandsProvider: CommandsProvider {
        commandId: documentData.documentTypeId;
        documentUuid: documentData.uuid;
    }

    property DocumentWorkspaceCommandsDelegateBase commandsDelegate: DocumentWorkspaceCommandsDelegateBase {
        documentPtr: documentData;
    }

    signal saved();

    Component.onCompleted: {
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(uuid + "CommandActivated", commandsDelegate.commandHandle);
    }

    onVisibleChanged: {
        if (visible){
            commandsProvider.updateGui();
        }
    }

    onUuidChanged: {
        console.log("onUuidChanged", uuid);

        if (uuid !== ""){
            Events.unSubscribeAllFromSlot(commandsDelegate.commandHandle);
            Events.subscribeEvent(uuid + "CommandActivated", commandsDelegate.commandHandle);
        }
    }

    onDocumentModelChanged: {
        console.log("onDocumentModelChanged", documentModel.toJSON());

        if (documentModel.ContainsKey("Id")){
            documentId = documentModel.GetData("Id")
        }
        else{
            console.warn("The document model does not contain an identifier");
        }

        updateGui();
    }

    onIsDirtyChanged: {
        commandsProvider.setCommandIsEnabled("Save", isDirty);
    }

    QtObject {
        id: internal;

        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;
    }

    Connections {
        target: documentModel;

        function onDataChanged(){
            console.log("documentModel onDataChanged");
            documentData.onModelChanged();
        }
    }

    function onModelChanged()
    {
        if (internal.blockingUpdateGui){
            return;
        }

        isDirty = true;

        doUpdateGui();
    }

    function updateGui()
    {
    }

    function updateModel()
    {
    }

    function doUpdateModel()
    {
        if (readOnly || internal.blockingUpdateModel){
            return;
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;
    }

    function doUpdateGui()
    {
        if (readOnly){
            return;
        }

        internal.blockingUpdateModel = true;

        updateGui();

        internal.blockingUpdateModel = false;
    }

    function updateCommands(){
        commandsProvider.updateModel();
    }

    function documentCanBeSaved(){
        return true;
    }
}
