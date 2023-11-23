import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;


    function getAvailableUndoSteps()
    {
        return internal.m_undoStack.length;
    }


    function getAvailableRedoSteps()
    {
        return internal.m_redoStack.length;
    }


    function resetUndo()
    {
        internal.m_undoStack = []
        internal.m_redoStack = []
    }


    function doUndo(steps = 1)
    {
        doListShift(steps, internal.m_undoStack, internal.m_redoStack);
    }


    function doRedo(steps = 1)
    {
        doListShift(steps, internal.m_redoStack, internal.m_undoStack);
    }


    function doListShift(steps, fromList, toList)
    {
        internal.m_isBlocked = true;

        if ((steps > 0) && (fromList.length >= steps)){
            for (let i = 1; i < steps; ++i){
                toList.push(fromList.pop())
            }
        }

        internal.m_isBlocked = false;
    }


    function registerModel(model)
    {
        if (modelIsRegistered()){
            console.error("Model is already registered in the undo manager");

            return;
        }

        internal.m_observerModel = model;
        internal.m_undoStack.push(model.CopyMe())
    }


    function unregisterModel()
    {
        internal.m_observerModel = null;
        resetUndo();
    }


    function modelIsRegistered()
    {
        return internal.m_observerModel != null;
    }


    function beginChanges()
    {
        if (!modelIsRegistered()){
            console.error("Unable to begin changes. Model is not registered");

            return;
        }

        if (internal.m_isBlocked){
            console.error("The changes in the undo manager have already started");

            return;
        }

        internal.m_isBlocked = true;
    }


    function endChanges()
    {
        if (!internal.m_isBlocked){
            console.error("The changes in the undo manager have not started");

            return;
        }

        makeChanges();

        internal.m_isBlocked = false;
    }


    function makeChanges()
    {
        if (!modelIsRegistered()){
            console.error("Unable to make changes. Model is not registered");

            return;
        }

        internal.m_undoStack.push(internal.m_observerModel.CopyMe())
    }


    QtObject {
        id: internal;

        property bool m_isBlocked: false;
        property TreeItemModel m_observerModel: null;

        property var m_undoStack: [];
        property var m_redoStack: [];
    }


    Connections {
        target: internal.m_observerModel;

        function onDataChanged(){
            if (internal.m_isBlocked){
                return;
            }

            internal.m_undoStack.push(internal.m_observerModel.CopyMe())
        }
    }
}


