import QtQuick 2.12
import Acf 1.0;

QtObject {
    id: undoRedoManager;

    // Model before saving
    property TreeItemModel mainModel: TreeItemModel {};
    property TreeItemModel observedModel: null;
    property DocumentBase documentBase: null;
    property UndoRedo undoRedo: UndoRedo {
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

    property Component treeItemModelComp: Component {
        TreeItemModel {}
    }

    property bool transaction: false;
    property bool modelIsRegistered: false;

    signal modelStateChanged();

    Component.onDestruction: {
        if (undoRedoManager.documentBase != null){
            undoRedoManager.documentBase.saved.disconnect(undoRedoManager.documentSaved);
            undoRedoManager.documentBase.commandsDelegateLoaded.disconnect(undoRedoManager.connectCommandHandle);
            undoRedoManager.disconnectCommandHandle();
        }
    }

    onDocumentBaseChanged: {
        if (undoRedoManager.documentBase != null){
            undoRedoManager.documentBase.saved.connect(undoRedoManager.documentSaved);
            undoRedoManager.documentBase.commandsDelegateLoaded.connect(undoRedoManager.connectCommandHandle);
        }
    }

    function connectCommandHandle(){
        if (undoRedoManager.documentBase != null && undoRedoManager.documentBase.commandsDelegate){
            undoRedoManager.documentBase.commandsDelegate.commandActivated.connect(undoRedoManager.commandHandle);
        }
    }

    function disconnectCommandHandle(){
        if (undoRedoManager.documentBase != null && undoRedoManager.documentBase.commandsDelegate){
            undoRedoManager.documentBase.commandsDelegate.commandActivated.disconnect(undoRedoManager.commandHandle);
        }
    }

    function documentSaved(){
        undoRedoManager.mainModel.Copy(undoRedoManager.documentBase.documentModel);
    }

    function beginChanges(){
        if (undoRedoManager.transaction || !undoRedoManager.modelIsRegistered){
            console.warn("UndoRedoManager beginChanges failed!");
            return;
        }

        undoRedoManager.transaction = true;
    }

    function endChanges(){
        if (!undoRedoManager.transaction || !undoRedoManager.modelIsRegistered){
            console.warn("UndoRedoManager endChanges failed!");
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

            let copyModel = undoRedoManager.getCopyFromModel(undoRedoManager.observedModel);
            undoRedo.addModel(copyModel);

            undoRedoManager.modelIsRegistered = true;
        }
    }

    function getCopyFromModel(model){
        let emptyModel = treeItemModelComp.createObject(undoRedo);
        emptyModel.Copy(model)

        return emptyModel;
    }

    function modelUpdated(){
        if (!undoRedoManager.transaction){
            let copyModel = undoRedoManager.getCopyFromModel(undoRedoManager.observedModel);

            if (undoRedo.undoStack.length >= 1){
                let lastModel = undoRedo.undoStack[undoRedo.undoStack.length - 1];
                let isEqual = copyModel.IsEqualWithModel(lastModel);
                if (isEqual){
                    return;
                }
            }

            undoRedo.addModel(copyModel);
        }
    }

    function checkCommandMode(){
        let isEnabled = undoRedo.undoStack.length > 1;
        undoRedoManager.documentBase.commandsProvider.setCommandIsEnabled("Undo", isEnabled);

        isEnabled = undoRedo.redoStack.length > 0;
        undoRedoManager.documentBase.commandsProvider.setCommandIsEnabled("Redo", isEnabled);
    }

    function equalWithMainModel(externModel){
        let isEqual = undoRedoManager.mainModel.IsEqualWithModel(externModel);

        return isEqual;
    }

    function commandHandle(commandId){
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
            let isEqual = undoRedoManager.equalWithMainModel(result);
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
        undoRedoManager.observedModel.Copy(obj);
    }
}
