import QtQuick 2.12
import Acf 1.0;

import "../../Acf/core.js" as Lodash

Item {
    id: undoRedoManager;

    property string commandsId;
    property TreeItemModel model: documentModel;
    property Item editorItem;

    signal modelParsed();
    signal commandActivated(string commandId);

    onModelChanged: {
        console.log("undoRedoManager onModelChanged");

        undoRedo.addModel(model);

        model.modelChanged.connect(modelUpdated);
    }

    Timer {
        id: timerCheckModel;

        interval: 10;

        onTriggered: {
            let newModel = JSON.stringify(model)

            let startModel = undoRedo.undoStack[0];
            if (_.isEqual(newModel, startModel)){
                commandsDelegate.removeChanges();
            }
        }
    }

    onModelParsed: {
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
        undoRedo.addModel(model);
        timerCheckModel.start();
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

        timerCheckModel.start();
    }

    function commandHandle(commandId){
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
        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        model.Parse(json);

        modelParsed();

        model.modelChanged.connect(modelUpdated);
    }
}
