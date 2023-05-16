import QtQuick 2.0
import Acf 1.0;

QtObject {
    id: undoRedo;

    property var undoStack: [];
    property var redoStack: [];

    signal undoChanged();
    signal redoChanged();
    signal modelAdded();

    function undo(){
        if (undoRedo.undoStack.length > 1){

            let obj = undoRedo.undoStack.pop();
            undoRedo.redoStack.push(obj);

            undoRedo.undoChanged();

            return undoRedo.undoStack[undoRedo.undoStack.length - 1];
        }

        return null;
    }

    function redo(){
        if (undoRedo.redoStack.length > 0){

            let obj = undoRedo.redoStack.pop();
            undoRedo.undoStack.push(obj);

            undoRedo.redoChanged();

            return obj;
        }

        return null;
    }

    function addModel(obj){
        if (!obj){
            return;
        }
        console.log("addModel", obj.toJSON());

        undoRedo.undoStack.push(obj);

        if (undoRedo.redoStack.length > 0){
            undoRedo.redoStack = [];
        }

        undoRedo.modelAdded();
    }
}
