import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0


Item {
    id: documentData;

    property string uuid;
    property string documentId;
    property string documentTypeId;

    property bool isDirty: false;
    property bool readOnly: false;
    property bool documentCompleted: true;

    property TreeItemModel documentModel: TreeItemModel {};
    property UndoRedoManager undoManagerPtr: UndoRedoManager {
        onModelChanged: {
            let undoSteps = getAvailableUndoSteps();
            let redoSteps = getAvailableRedoSteps();

            documentData.commandsProvider.setCommandIsEnabled("Undo", undoSteps > 0);
            documentData.commandsProvider.setCommandIsEnabled("Redo", redoSteps > 0);
        }

        onUndo: {
            checkDocumentModel();
        }

        onRedo: {
            checkDocumentModel()
        }

        function checkDocumentModel(){
            let currentStateModel = getStandardModel();
            if (currentStateModel){
                let isEqual = currentStateModel.IsEqualWithModel(documentData.documentModel);
                documentData.isDirty = !isEqual;
            }
        }
    }

    property var documentManagerPtr: null;

    property CommandsProvider commandsProvider: CommandsProvider {
        commandId: documentData.documentTypeId;
        documentUuid: documentData.uuid;
    }

    property DocumentWorkspaceCommandsDelegateBase commandsDelegate: DocumentWorkspaceCommandsDelegateBase {
        documentPtr: documentData;
    }

    signal saved();

    signal startLoading();
    signal stopLoading();

    Component.onCompleted: {
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(uuid + "CommandActivated", commandsDelegate.commandHandle);
    }

    onSaved: {
        if (undoManagerPtr){
            undoManagerPtr.setStandardModel(documentData.documentModel);
        }
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

            if (undoManagerPtr){
                Events.subscribeEvent(uuid + "CommandActivated", undoManagerPtr.commandHandle);
            }
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

        // If model is empty then doUpdateModel
        if (documentModel.GetItemsCount() === 0){
            doUpdateModel();
        }

        let callback = function(){
            doUpdateGui();

            if (undoManagerPtr){
                undoManagerPtr.registerModel(documentModel);
            }

            onDocumentCompletedChanged.disconnect(callback);
        }

        if (documentCompleted){
            callback();
        }
        else{
            onDocumentCompletedChanged.connect(callback);
        }
    }

    onIsDirtyChanged: {
        console.log("onIsDirtyChanged", isDirty);
        commandsProvider.setCommandIsEnabled("Save", isDirty);
    }

    Connections {
        target: documentModel;

        function onDataChanged(){
            console.log("documentModel onDataChanged", documentModel.toJSON());
            documentData.onModelChanged();
        }
    }

    function onModelChanged()
    {
        console.log("onModelChanged", internal.blockingUpdateGui);

        if (internal.blockingUpdateGui){
            internal.countIncomingChanges++;
            return;
        }

        isDirty = true;

        doUpdateGui();
    }

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

    function doUpdateModel()
    {
        console.log("doUpdateModel", internal.blockingUpdateModel);

        if (readOnly || internal.blockingUpdateModel){
            return;
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;
    }

    function doUpdateGui()
    {
        console.log("doUpdateGui");

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


    QtObject {
        id: internal;

        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;

        property int countIncomingChanges: 0;

        onBlockingUpdateGuiChanged: {
            console.log("onBlockingUpdateGuiChanged", blockingUpdateGui);
            if (blockingUpdateGui){
                documentData.startLoading();
            }
            else{
                documentData.stopLoading();
            }

            if (!blockingUpdateGui && countIncomingChanges > 0){
                countIncomingChanges = 0;
                documentData.onModelChanged();
            }
        }

        onBlockingUpdateModelChanged: {
            console.log("onBlockingUpdateModelChanged", blockingUpdateModel);

            if (blockingUpdateModel){
                documentData.startLoading();
            }
            else{
                documentData.stopLoading();
            }
        }
    }
}
