import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;

    property string commandsId;
    property var commandsDelegate;

    property TreeItemModel observedModel;

    signal modelStateChanged();
    signal commandActivated(string commandId);

    property bool isDirty: false;
    property bool transaction: false;

    function beginChanges(){
        console.log("UndoRedoManager beginChanges");
        if (isDirty){
            console.assert(isDirty == true,  "beginChanges():: beginChanges isDirty == true");

            return;
        }

        isDirty = true;

        transaction = true;
    }

    function endChanges(){
        console.log("UndoRedoManager endChanges");
        if (!isDirty){
            console.assert(isDirty == false,  "endChanges():: beginChanges isDirty = false");

            return;
        }
        else{
            isDirty = false;

            modelUpdated();

            transaction = false
        }
    }

    function registerModel(model){
        if (undoRedoManager.observedModel != model){
            undoRedoManager.observedModel = model;

            undoRedo.addModel(observedModel);

            observedModel.modelChanged.connect(modelUpdated);
        }
    }

    Timer {
        id: timerCheckModel;

        interval: 10;

        onTriggered: {
            let newModel = JSON.stringify(undoRedoManager.observedModel)

            let startModel = undoRedo.undoStack[0];
            if (_.isEqual(newModel, startModel)){
                undoRedoManager.commandsDelegate.removeChanges();
            }
        }
    }

    onModelStateChanged: {
        if (undoRedo.undoStack.length == 1){
            commandsDelegate.removeChanges();
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onCommandsIdChanged: {
        Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onVisibleChanged: {
        console.log("undoRedoManager onVisibleChanged", visible);
        if (undoRedoManager.visible){
            observedModel.modelChanged.connect(modelUpdated);
            Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
        }
        else{
            observedModel.modelChanged.disconnect(modelUpdated);
            Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle)
        }
    }

    function modelUpdated(){
        console.log("undoRedoManager modelUpdated", isDirty);
        if (transaction &&!isDirty){
            undoRedo.addModel(observedModel);
        }
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

    function checkCommandMode(){
        let state = undoRedo.undoStack.length > 1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Undo", state);

        state = undoRedo.redoStack.length > 0 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Redo", state);

        timerCheckModel.start();
    }

    function commandHandle(commandId){
        console.log("undoRedoManager commandHandle", commandId);

        let isEnabled = commandsProvider.commandIsEnabled(commandId);
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

        commandActivated(commandId);

        if (result !== null){

            createModel(result);

            modelStateChanged();
        }
    }

    function createModel(json){
        observedModel.modelChanged.disconnect(modelUpdated);
        console.log("createModel", json);

        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        observedModel.CreateFromJson(json);

        observedModel.modelChanged.connect(modelUpdated);
    }
}
