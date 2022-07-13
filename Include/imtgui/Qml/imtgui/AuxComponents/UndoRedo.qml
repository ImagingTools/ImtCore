import QtQuick 2.0
import Acf 1.0;

Item {
    id: undoRedo;

    property Item rootItem;
    property Item multiDocViewItem;

    property int undoStackCountObjects: 0;
    property var undoStack: [];
    property var redoStack: [];

    signal undoChanged();
    signal redoChanged();
    signal modelAdded();

    function undo(){
        console.log("UndoRedo undo");

        if (undoRedo.undoStack.length > 1){

            undoRedo.undoStackCountObjects--;
            let obj = undoRedo.undoStack.pop();
            undoRedo.redoStack.push(obj);

            undoRedo.updateStatesCommands();
            undoRedo.multiDocViewItem.enableChanges();
            undoRedo.undoChanged();

            return undoRedo.undoStack[undoRedo.undoStack.length - 1];
        }

        return null;
    }

    function undoStackIsEmpty(){
        return undoRedo.undoStack.length == 1;
    }

    function redoStackIsEmpty(){
        return undoRedo.redoStack.length == 0;
    }

    function redo(){
        console.log("UndoRedo redo");

        if (undoRedo.redoStack.length > 0){
            undoRedo.undoStackCountObjects++;
            let obj = undoRedo.redoStack.pop();
            undoRedo.undoStack.push(obj);

            undoRedo.updateStatesCommands();
            undoRedo.multiDocViewItem.enableChanges();
            undoRedo.redoChanged();

            return obj;
        }

        return null;
    }

    function addModel(obj){
        undoRedo.undoStackCountObjects++;
        console.log("addModel", JSON.stringify(obj));
        undoRedo.undoStack.push(JSON.stringify(obj));
        undoRedo.redoStack = [];

        undoRedo.updateStatesCommands();
        if (undoRedo.undoStack.length > 1){
            undoRedo.multiDocViewItem.enableChanges();
        }

        undoRedo.modelAdded();
    }

    function updateStatesCommands(){
        if (!undoRedo.rootItem){
            return;
        }

        if (undoRedo.undoStack.length > 1){
            undoRedo.rootItem.setModeMenuButton("Undo", "Normal");
        }
        else{
            undoRedo.rootItem.setModeMenuButton("Undo", "Disabled");
        }

        if (undoRedo.redoStackIsEmpty()){
            undoRedo.rootItem.setModeMenuButton("Redo", "Disabled");
        }
        else{
            undoRedo.rootItem.setModeMenuButton("Redo", "Normal");
        }
    }
}
