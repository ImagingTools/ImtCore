import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;

    property string commandsId;
    property TreeItemModel model;
    property Item editorItem;

    signal modelParsed();
    signal commandActivated(string commandId);

    onModelChanged: {
        console.log("undoRedoManager onModelChanged");
//        undoRedo.addModel(model);

//        model.modelChanged.connect(modelUpdated);
    }

    Component.onDestruction: {
//        model.modelChanged.disconnect(modelUpdated);
        Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onCommandsIdChanged: {
        console.log("UndoRedo onCommandsIdChanged", undoRedoManager.commandsId);
        Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onVisibleChanged: {
        console.log("UndoRedoManager onVisibleChanged", undoRedoManager.visible);
        if (undoRedoManager.visible){
            model.modelChanged.connect(modelUpdated);
            Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
        }
        else{
            model.modelChanged.disconnect(modelUpdated);
            Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle)
        }
    }

    function modelUpdated(){
        console.log("undoRedoManager modelUpdated");
        undoRedo.addModel(model);
    }

    Shortcut {
        sequence: "Ctrl+Z";
        onActivated: {
            commandActivated("Undo");
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";
        onActivated: {
            commandActivated("Redo");
        }
    }

    UndoRedo {
        id: undoRedo;

        onModelAdded: {
            checkCommandMode();
        }

        onUndoChanged: {
            checkCommandMode();
        }

        onRedoChanged: {
            checkCommandMode();
        }
    }

    function checkCommandMode(){
        let state = undoRedo.undoStack.length > 1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Undo", state);

        state = undoRedo.redoStack.length > 0 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Redo", state);
    }

    function commandHandle(commandId){
        console.log("UndoRedo commandHandle", commandId, undoRedo);

        let result = null;
        if (commandId === "Undo"){
            result = undoRedo.undo();
        }
        else if (commandId === "Redo"){
            result = undoRedo.redo();
        }

        commandActivated(commandId);

        if (result !== null){

            parseModel(result);
        }
    }

    function parseModel(json){
        model.modelChanged.disconnect(modelUpdated);
        console.log("UndoRedo parseModel", json);
        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        model.Parse(json);

        modelParsed();

        model.modelChanged.connect(modelUpdated);
    }
}
