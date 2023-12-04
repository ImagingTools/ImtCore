import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;

    signal modelChanged();
    signal undo();
    signal redo();


    function getCurrentStateModel()
    {
        if (internal.m_undoStack.length > 0){
            return internal.m_undoStack[internal.m_undoStack.length - 1];
        }

        return null;
    }


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

        modelChanged();
    }


    function doUndo(steps)
    {
        console.log("doUndo");

        printInfo();

        internal.m_isBlocked = true;

        internal.m_redoStack.push(internal.m_observedModel.CopyMe());

        let prevStateModel = internal.m_undoStack.pop();
        if (internal.m_observedModel.Copy(prevStateModel)){
            internal.m_observedModel.Refresh();
        }

        internal.m_beginStateModel = internal.m_observedModel.CopyMe();

        internal.m_isBlocked = false;

        modelChanged();
//        doListShift(steps, internal.m_undoStack, internal.m_redoStack);

        printInfo();

        undo();
    }


    function doRedo(steps)
    {
        console.log("doRedo");

        printInfo();

        internal.m_isBlocked = true;

        internal.m_undoStack.push(internal.m_observedModel.CopyMe());

        let nextStateModel = internal.m_redoStack.pop();
        if (internal.m_observedModel.Copy(nextStateModel)){
            internal.m_observedModel.Refresh();
        }

        internal.m_beginStateModel = internal.m_observedModel.CopyMe();

        internal.m_isBlocked = false;

        modelChanged();
//        doListShift(steps, internal.m_redoStack, internal.m_undoStack);

        printInfo();
        redo();
    }


    function doListShift(steps, fromList, toList)
    {
        if (internal.m_isBlocked){
            return;
        }

        if ((steps > 0) && (fromList.length >= steps)){
            internal.m_isBlocked = true;

            printInfo();

            let stateModel = fromList[fromList.length - steps];
            if (internal.m_observedModel.Copy(stateModel)){
                internal.m_observedModel.Refresh();
            }
            else{
                console.log("Unable to copy observer model from current state model");
            }

            for (let i = 0; i < steps; ++i){
                toList.push(fromList.pop())
            }

            modelChanged();

            printInfo();

            internal.m_isBlocked = false;
        }
    }


    function registerModel(model)
    {
        if (modelIsRegistered()){
            console.error("Model is already registered in the undo manager");

            return;
        }

        resetUndo();

        internal.m_beginStateModel = model.CopyMe();

        internal.m_observedModel = model;

        internal.m_observedModel.dataChanged.connect(onDataChanged);

        setStandardModel(model);
    }


    function unregisterModel()
    {
        internal.m_observedModel.dataChanged.disconnect(onDataChanged);

        internal.m_observedModel = null;
        resetUndo();
    }


    function modelIsRegistered()
    {
        return internal.m_observedModel != null;
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

        internal.m_beginStateModel = internal.m_observedModel.CopyMe();

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

        if (internal.m_beginStateModel != null){
            let copiedModel = internal.m_beginStateModel.CopyMe();

            internal.m_undoStack.push(copiedModel)
            internal.m_redoStack = []

            modelChanged();
        }

        printInfo();
    }


    function commandHandle(commandId)
    {
        if (commandId === "Undo"){
            doUndo()
        }
        else if (commandId === "Redo"){
            doRedo();
        }
    }


    function setStandardModel(model)
    {
        if (!modelIsRegistered()){
            console.error("Unable to set standard model. Model is not registered");

            return;
        }

        let copiedModel = model.CopyMe();
        internal.m_defaultStateModel = copiedModel;
    }


    // The first model in the stack is a copy of the observed model

    function getStandardModel()
    {
        return internal.m_defaultStateModel;
    }


    QtObject {
        id: internal;

        property bool m_isBlocked: false;

        property TreeItemModel m_observedModel: null;
        property TreeItemModel m_beginStateModel: null;
        property TreeItemModel m_defaultStateModel: null;

        property var m_undoStack: [];
        property var m_redoStack: [];
    }

    function onDataChanged(){
        if (internal.m_isBlocked){
            return;
        }

        console.error("Undo manager onDataChanged");

        undoRedoManager.makeChanges();

        internal.m_beginStateModel = internal.m_observedModel.CopyMe();
    }


//    Connections {
//        target: internal.m_observedModel;

//        function onDataChanged(){
//            if (internal.m_isBlocked){
//                return;
//            }

//            console.error("Undo manager onDataChanged");

//            undoRedoManager.makeChanges();

//            internal.m_beginStateModel = internal.m_observedModel.CopyMe();
//        }
//    }

    function printInfo(){
        console.log("printInfo");
        console.log("---------UNDO-----------");
        for (let i = 0; i < internal.m_undoStack.length; ++i){
            console.log(i, internal.m_undoStack[i].toJSON());
        }
        console.log("-------------------------");

        console.log("---------REDO-----------");
        for (let i = 0; i < internal.m_redoStack.length; ++i){
            console.log(i, internal.m_redoStack[i].toJSON());
        }
        console.log("-------------------------");
    }
}


