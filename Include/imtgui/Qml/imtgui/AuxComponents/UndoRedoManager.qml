import QtQuick 2.0
import Acf 1.0;

Item {
    id: undoRedoManager;

    property string commandsId;
    property TreeItemModel model;

    signal modelParsed();

    onModelChanged: {
        undoRedo.addModel(model);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onCommandsIdChanged: {
        console.log("UndoRedo onCommandsIdChanged", undoRedoManager.commandsId);
        Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
    }

    onVisibleChanged: {
        if (undoRedoManager.visible){
            Events.subscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle);
        }
        else{
            Events.unSubscribeEvent(undoRedoManager.commandsId + "CommandActivated", undoRedoManager.commandHandle)
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
        commandsProvider.changeCommandMode("Save", state);

        let suffix = "";
        if (state === "Normal"){
            suffix = "*";
        }

        multiDocView.updatePageTitle({"ItemId": "", "Title": model.GetData("Name") + suffix});

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
//        else if (commandId === "Save"){
//            if (itemId != ""){
//                undoRedo.undoStack = []
//                undoRedo.addModel(model);
//            }
//        }

        if (result !== null){
            parseModel(result);
        }
    }

    function parseModel(json){
        console.log("UndoRedo parseModel", json);
        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        model.Parse(json);

        modelParsed();
    }
}
