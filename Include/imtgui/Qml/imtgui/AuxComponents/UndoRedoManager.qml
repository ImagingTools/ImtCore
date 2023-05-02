import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;

    property string commandsId;
    property var commandsDelegate;

    // Model before saving
    property TreeItemModel mainModel: TreeItemModel {};

    property TreeItemModel observedModel: null;

    property Item documentBase: null;

    property bool transaction: false;

    signal modelStateChanged();

    Component.onDestruction: {
        Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);

        if (undoRedoManager.documentBase != null){
            undoRedoManager.documentBase.saved.disconnect(undoRedoManager.documentSaved);
        }
    }

    onCommandsIdChanged: {
        Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onVisibleChanged: {
        console.log("undoRedoManager onVisibleChanged", undoRedoManager.visible);
        if (undoRedoManager.visible){
            if (undoRedoManager.observedModel != null){
                Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
            }
        }
        else{
            if (undoRedoManager.observedModel != null){
                Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
            }
        }
    }

    onDocumentBaseChanged: {
        if (undoRedoManager.documentBase != null){
            undoRedoManager.documentBase.saved.connect(undoRedoManager.documentSaved);
        }
    }

    function documentSaved(){
        console.log("UndoRedoManager documentSaved");
        undoRedoManager.mainModel.Copy(undoRedoManager.documentBase.documentModel);
    }

    function beginChanges(){
        console.log("UndoRedoManager beginChanges");
        if (undoRedoManager.transaction){
            console.assert(undoRedoManager.transaction == true,  "beginChanges():: beginChanges transaction == true");

            return;
        }

        undoRedoManager.transaction = true;
    }

    function endChanges(){
        console.log("UndoRedoManager endChanges");
        if (!undoRedoManager.transaction){
            console.assert(undoRedoManager.transaction == false,  "endChanges():: beginChanges transaction = false");

            return;
        }
        else{
            undoRedoManager.transaction = false;

            undoRedoManager.modelUpdated();
        }
    }

    function registerModel(model){
        if (!model){
            return;
        }

        if (undoRedoManager.observedModel !== model){
            undoRedoManager.observedModel = model;

            undoRedoManager.mainModel.Copy(undoRedoManager.observedModel);

            undoRedo.addModel(undoRedoManager.observedModel);
        }
    }

    function modelUpdated(){
        console.log("undoRedoManager modelUpdated", undoRedoManager.transaction);
        if (!undoRedoManager.transaction){
            undoRedo.addModel(undoRedoManager.observedModel);
        }
    }

    function checkCommandMode(){
        let isEnabled = undoRedo.undoStack.length > 1;
        undoRedoManager.documentBase.commandsProvider.setCommandIsEnabled("Undo", isEnabled);

        isEnabled = undoRedo.redoStack.length > 0;
        undoRedoManager.documentBase.commandsProvider.setCommandIsEnabled("Redo", isEnabled);
    }

    function commandHandle(commandId){
        console.log("undoRedoManager commandHandle", commandId);
        let isEnabled = undoRedoManager.documentBase.commandsProvider.commandIsEnabled(commandId);
        if (!isEnabled){
            return;
        }

        let result = null;
        if (commandId === "Undo"){
            result = undoRedo.undo();
        }
        else if (commandId === "Redo"){
            result = undoRedo.redo();
        }

        if (result !== null){
            let isEqual = undoRedoManager.mainModel.IsEqualWithModel(result);
            console.log("isEqual", isEqual);
            if (undoRedoManager.documentBase){
                undoRedoManager.documentBase.isDirty = !isEqual;

                undoRedoManager.documentBase.blockUpdatingModel = true;
            }

            undoRedoManager.createModel(result);
            undoRedoManager.modelStateChanged();

            if (undoRedoManager.documentBase){
                undoRedoManager.documentBase.blockUpdatingModel = false;
            }
        }
    }

    function createModel(obj){
        console.log("createModel", JSON.stringify(obj));
        undoRedoManager.observedModel.Copy(obj);
    }

    UndoRedo {
        id: undoRedo;

        onModelAdded: {
            undoRedoManager.checkCommandMode();
        }

        onUndoChanged: {
            undoRedoManager.checkCommandMode();
        }

        onRedoChanged: {
            undoRedoManager.checkCommandMode();
        }
    }
}
