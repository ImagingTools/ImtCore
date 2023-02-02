import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;

    property string commandsId;
    property var commandsDelegate;

    property TreeItemModel observedModel;

    property Item documentBase: null;


    property bool isDirty: false;
    property bool transaction: false;

    signal modelStateChanged();
    signal commandActivated(string commandId);

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
        console.log("undoRedoManager onVisibleChanged", undoRedoManager.visible);
        if (undoRedoManager.visible){
            observedModel.modelChanged.connect(undoRedoManager.modelUpdated);
            Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
        }
        else{
            observedModel.modelChanged.disconnect(undoRedoManager.modelUpdated);
            Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
        }
    }

    function beginChanges(){
        console.log("UndoRedoManager beginChanges");
        if (undoRedoManager.isDirty){
            console.assert(undoRedoManager.isDirty == true,  "beginChanges():: beginChanges isDirty == true");

            return;
        }

        undoRedoManager.isDirty = true;

        undoRedoManager.transaction = true;
    }

    function endChanges(){
        console.log("UndoRedoManager endChanges");
        if (!undoRedoManager.isDirty){
            console.assert(undoRedoManager.isDirty == false,  "endChanges():: beginChanges isDirty = false");

            return;
        }
        else{
            undoRedoManager.isDirty = false;

            undoRedoManager.modelUpdated();

            undoRedoManager.transaction = false;
        }
    }

    function registerModel(model){
        if (undoRedoManager.observedModel != model){
            undoRedoManager.observedModel = model;

            undoRedo.addModel(undoRedoManager.observedModel);

            undoRedoManager.observedModel.modelChanged.connect(undoRedoManager.modelUpdated);
        }
    }


    function modelUpdated(){
        console.log("undoRedoManager modelUpdated", undoRedoManager.isDirty);
        if (undoRedoManager.transaction &&!undoRedoManager.isDirty){
            undoRedo.addModel(undoRedoManager.observedModel);
        }
    }

    function checkCommandMode(){
        let isEnabled = undoRedo.undoStack.length > 1;
        undoRedoManager.documentBase.commandsProvider.setCommandIsEnabled("Undo", isEnabled);

        isEnabled = undoRedo.redoStack.length > 0;
        undoRedoManager.documentBase.commandsProvider.setCommandIsEnabled("Redo", isEnabled);

        timerCheckModel.start();
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

        undoRedoManager.commandActivated(commandId);

        if (result !== null){

            undoRedoManager.createModel(result);

            undoRedoManager.modelStateChanged();
        }
    }

    function createModel(json){
        undoRedoManager.observedModel.modelChanged.disconnect(undoRedoManager.modelUpdated);
        console.log("createModel", json);

        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        undoRedoManager.observedModel.CreateFromJson(json);

        undoRedoManager.observedModel.modelChanged.connect(undoRedoManager.modelUpdated);
    }

    Timer {
        id: timerCheckModel;

        interval: 10;

        onTriggered: {
            if (!undoRedoManager.commandsDelegate){
                return;
            }

            let newModel = JSON.stringify(undoRedoManager.observedModel)

            let startModel = undoRedo.undoStack[0];
            if (_.isEqual(newModel, startModel)){
                undoRedoManager.commandsDelegate.removeChanges();
            }
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

}
