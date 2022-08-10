import QtQuick 2.0
import Acf 1.0;

Item {
    id: undoRedo;

    property var undoStack: [];
    property var redoStack: [];

    signal undoChanged();
    signal redoChanged();
    signal modelAdded();

    function undo(){
        console.log("UndoRedo undo");

        if (undoRedo.undoStack.length > 1){

            let obj = undoRedo.undoStack.pop();
            undoRedo.redoStack.push(obj);

            undoRedo.undoChanged();

            return undoRedo.undoStack[undoRedo.undoStack.length - 1];
        }

        return null;
    }

    function redo(){
        console.log("UndoRedo redo");

        if (undoRedo.redoStack.length > 0){

            let obj = undoRedo.redoStack.pop();
            undoRedo.undoStack.push(obj);

            undoRedo.redoChanged();

            return obj;
        }

        return null;
    }

    function addModel(obj){
        console.log("UndoRedo addModel", JSON.stringify(obj));

        undoRedo.undoStack.push(JSON.stringify(obj));
        undoRedo.redoStack = [];

        undoRedo.modelAdded();
    }
}
