import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtgui 1.0


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
        commandId: documentTypeId;
        uuid: documentData.uuid;

        property bool documentVisible: documentData.visible && uuid !== "";
        onDocumentVisibleChanged: {
            if (commandsModel == null){
                commandsProvider.additionInputParams = getAdditionalInputParams();

                updateModel();
            }

            if (documentVisible){
                updateGui()
            }
            else{
                clearGui();
            }
        }

        onCommandsModelChanged: {
            if (documentVisible){
                updateGui()
            }
        }
    }

    property DocumentWorkspaceCommandsDelegateBase commandsDelegate: DocumentWorkspaceCommandsDelegateBase {
        documentPtr: documentData;
    }

    signal saved();
    signal startLoading();
    signal stopLoading();

    Component.onDestruction: {
        Events.unSubscribeEvent(uuid + "CommandActivated", commandsDelegate.commandHandle);
    }

    onStartLoading: {
        loading.start();
    }

    onStopLoading: {
        loading.stop();
    }

    onSaved: {
        if (undoManagerPtr){
            undoManagerPtr.setStandardModel(documentData.documentModel);
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

        beginDocumentModelChanged();

        if (documentModel.ContainsKey("Id")){
            documentId = documentModel.GetData("Id")
        }
        else{
            console.warn("The document model does not contain an identifier");
        }

        // If model is empty then doUpdateModel

        console.log("GetItemsCount", documentModel.GetItemsCount());

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

        endDocumentModelChanged();
    }

    onIsDirtyChanged: {
        console.log("onIsDirtyChanged", isDirty);
        commandsProvider.setCommandIsEnabled("Save", isDirty);
    }

    Connections {
        target: documentModel;

        function onDataChanged(){
            documentData.onModelChanged();
        }
    }

    function beginDocumentModelChanged(){}
    function endDocumentModelChanged(){}

    function getDocumentName()
    {
        let documentName = "";
        if (documentModel.ContainsKey("Name")){
            documentName = documentModel.GetData("Name");
        }

        return documentName;
    }

    function onModelChanged()
    {
        if (internal.blockingUpdateGui){
            internal.countIncomingChanges++;
            return;
        }

        console.log("onModelChanged", documentModel.toJSON());

        isDirty = true;

        doUpdateGui();
    }

    function getAdditionalInputParams()
    {
        console.log("Document getAdditionalInputParams");

        return {}
    }

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

    function doUpdateModel()
    {
        console.log("start doUpdateModel", JSON.stringify(documentModel));

        if (readOnly || internal.blockingUpdateModel){
            console.log("doUpdateModel cancelled");

            return;
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;

        console.log("finish doUpdateModel", JSON.stringify(documentModel));
    }

    function doUpdateGui()
    {
        console.log("start doUpdateGui");

        if (readOnly){
            return;
        }

        internal.blockingUpdateModel = true;

        updateGui();

        internal.blockingUpdateModel = false;

        console.log("finish doUpdateGui");
    }

    function updateCommands(){
        commandsProvider.additionInputParams = getAdditionalInputParams();
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

    Shortcut {
        sequence: "Ctrl+S";

        enabled: documentData.visible;

        onActivated: {
            console.log("Ctrl+S onActivated");
            Events.sendEvent(documentData.uuid + "CommandActivated", "Save");
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";

        enabled: documentData.visible;

        onActivated: {
            console.log("Ctrl+Z onActivated");
            Events.sendEvent(documentData.uuid + "CommandActivated", "Undo");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";

        enabled: documentData.visible;

        onActivated: {
            Events.sendEvent(documentData.uuid + "CommandActivated", "Redo");
        }
    }

    Loading {
        id: loading;

        z: parent.z + 1;

        anchors.fill: parent;

        visible: true;
    }
}
