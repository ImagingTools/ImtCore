import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtcontrols 1.0

// DocumentView
Item {
    id: documentData;

    property string uuid;
    property string documentId;
    property string documentName;
    property string documentTypeId;
    property var documentManagerPtr: null;

    property bool isDirty: false;
    property bool readOnly: false;
    property bool documentCompleted: true;

    property TreeItemModel documentModel: TreeItemModel {
        onDataChanged:{
            documentData.onModelChanged();
        }
    };

    property var commandsTemplate: [
        {Id: "Close"}
    ]

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

    property CommandsController commandsController: CommandsController {}

    property CommandsProvider commandsProvider: CommandsProvider {
//        commandId: documentData.documentTypeId;
        uuid: documentData.uuid;

        property bool documentVisible: documentData.visible && uuid !== "";
        onDocumentVisibleChanged: {
            if (commandsModel == null){
                documentData.commandsProvider.additionInputParams = documentData.getAdditionalInputParams();

                updateModel();
            }

            if (documentVisible){
                updateGui()
            }
        }

        onCommandsModelChanged: {
            if (documentVisible){
                updateGui()
            }
        }
    }

    property ViewCommandsDelegateBase commandsDelegate: ViewCommandsDelegateBase {
        documentPtr: documentData;
    }

    signal saved();
    signal startLoading();
    signal stopLoading();

    Component.onDestruction: {
        Events.unSubscribeEvent(uuid + "CommandActivated", commandsDelegate.commandHandle);

        documentData.documentModel.Clear();
    }

    onStartLoading: {
        Events.sendEvent("StartLoading");
    }

    onStopLoading: {
        Events.sendEvent("StopLoading");
    }

    onSaved: {
        isDirty = false;

        if (undoManagerPtr){
            undoManagerPtr.setStandardModel(documentData.documentModel);
        }
    }

    onUuidChanged: {
        if (uuid !== ""){
            Events.unSubscribeAllFromSlot(commandsDelegate.commandHandle);
            Events.subscribeEvent(uuid + "CommandActivated", commandsDelegate.commandHandle);

            if (undoManagerPtr){
                Events.subscribeEvent(uuid + "CommandActivated", undoManagerPtr.commandHandle);
            }
        }
    }

    onDocumentModelChanged: {
        let isEmpty = documentModel.GetItemsCount() === 0;

        beginDocumentModelChanged();

        if (documentModel.ContainsKey("Id")){
            documentId = documentModel.GetData("Id")
        }
        else{
            console.warn("The document model does not contain an identifier");
        }

        if (documentModel.ContainsKey("Name")){
            documentName = documentModel.GetData("Name")
        }
        else{
            console.warn("The document model does not contain a name");
        }

        // If model is empty then doUpdateModel
        if (isEmpty){
            doUpdateModel();
        }

        let callback = function(){
            doUpdateGui();

            if (undoManagerPtr){
                undoManagerPtr.registerModel(documentModel);
            }

            documentData.documentCompletedChanged.disconnect(callback);
        }

        if (documentCompleted){
            callback();
        }
        else{
            documentData.documentCompletedChanged.connect(callback);
        }

        endDocumentModelChanged();

        documentModel.dataChanged.connect(onModelChanged);
    }

    onIsDirtyChanged: {
        commandsProvider.setCommandIsEnabled("Save", isDirty);
    }

    function beginDocumentModelChanged(){}
    function endDocumentModelChanged(){}

    function getDocumentName()
    {
        return documentName;
    }

    function onModelChanged()
    {
        if (internal.blockingUpdateGui || internal.blockingUpdateModel){
            internal.countIncomingChanges++;
            return;
        }

        isDirty = true;

        doUpdateGui();
    }

    function getAdditionalInputParams()
    {
        return {}
    }

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

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
        commandsProvider.additionInputParams = getAdditionalInputParams();
        commandsProvider.updateModel();
    }

    function documentCanBeSaved(){
        return true;
    }

    function setBlockingUpdateModel(value){
        internal.blockingUpdateModel = value;
    }

    function setBlockingUpdateGui(value){
        internal.blockingUpdateGui = value;
    }

    QtObject {
        id: internal;

        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;

        property int countIncomingChanges: 0;

        onBlockingUpdateGuiChanged: {
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
            if (blockingUpdateModel){
                loading.start();
            }
            else{
                loading.stop();
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+S";
        enabled: documentData.visible;
        onActivated: {
            if (!documentData.isDirty){
                return
            }

            if (documentData.commandsProvider.commandExists("Save")){
                Events.sendEvent(documentData.uuid + "CommandActivated", "Save");
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+Z";
        enabled: documentData.visible;
        onActivated: {
            console.log("Ctrl+Z")
            let undoSteps = documentData.undoManagerPtr.getAvailableUndoSteps();

            if (undoSteps > 0){
                if (documentData.commandsProvider.commandExists("Undo")){
                    Events.sendEvent(documentData.uuid + "CommandActivated", "Undo");
                }
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";
        enabled: documentData.visible;
        onActivated: {
            console.log("Ctrl+Shift+Z")
            let redoSteps = documentData.undoManagerPtr.getAvailableRedoSteps();
            if (redoSteps > 0){
                if (documentData.commandsProvider.commandExists("Redo")){
                    Events.sendEvent(documentData.uuid + "CommandActivated", "Redo");
                }
            }
        }
    }

    Loading {
        id: loading;

        z: parent.z + 1;

        anchors.fill: parent;

        visible: true;
    }
}
