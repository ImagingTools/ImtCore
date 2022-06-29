import QtQuick 2.0
import Acf 1.0;

Item {
    id: undoRedo;

    property int undoStackCountObjects: 0;
    property var undoStack: [];
    property var redoStack: [];

    function undo(){
        console.log("UndoRedo undo");

        if (undoRedo.undoStack.length > 1){

            undoRedo.undoStackCountObjects--;
            let obj = undoRedo.undoStack.pop();
            undoRedo.redoStack.push(obj);

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

            return obj;
        }

        return null;
    }

    function addModel(obj){
        undoRedo.undoStackCountObjects++;
        undoRedo.undoStack.push(JSON.stringify(obj));
    }
}
