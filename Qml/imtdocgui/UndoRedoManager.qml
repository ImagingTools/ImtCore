import QtQuick 2.12
import Acf 1.0;

Item {
    id: undoRedoManager;

    property bool autoTracking: true; // automatic tracking of model changes

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


    function doUndo()
    {
        if (!modelIsRegistered()){
            return;
        }

        internal.m_isBlocked = true;

        internal.m_redoStack.push(internal.m_observedModel.toJson());

        let prevStateModel = internal.m_undoStack.pop();

        let copiedModel = internal.m_observedModel.copyMe();
        copiedModel.createFromJson(prevStateModel)

        internal.m_observedModel.copy(copiedModel);

        internal.m_beginStateModel = prevStateModel;

        modelChanged();
        undo();

        internal.m_isBlocked = false;
    }


    function doRedo(steps)
    {
        if (!modelIsRegistered()){
            return;
        }

        internal.m_isBlocked = true;

        internal.m_undoStack.push(internal.m_observedModel.toJson());

        let nextStateModel = internal.m_redoStack.pop();

        let copiedModel = internal.m_observedModel.copyMe();
        copiedModel.createFromJson(nextStateModel)
        internal.m_observedModel.copy(copiedModel);

//        internal.m_observedModel.createFromJson(nextStateModel)
        internal.m_beginStateModel = internal.m_observedModel.toJson();

        modelChanged();
        redo();

        internal.m_isBlocked = false;
    }


    function doListShift(steps, fromList, toList)
    {
        if (internal.m_isBlocked){
            return;
        }

        if ((steps > 0) && (fromList.length >= steps)){
            internal.m_isBlocked = true;

            let stateModel = fromList[fromList.length - steps];
            if (internal.m_observedModel.copy(stateModel)){
                internal.m_observedModel.refresh();
            }
            else{
                console.log("Unable to copy observer model from current state model");
            }

            for (let i = 0; i < steps; ++i){
                toList.push(fromList.pop())
            }

            modelChanged();

            internal.m_isBlocked = false;
        }
    }


    function registerModel(model)
    {
        if (modelIsRegistered()){
            console.warn("Model is already registered in the undo manager");

            unregisterModel();
        }

        resetUndo();

        internal.m_beginStateModel = model.toJson();

        internal.m_observedModel = model;

        if (autoTracking){
            internal.m_observedModel.modelChanged.connect(onModelChanged);
        }

        setStandardModel(model);
    }


    function unregisterModel()
    {
        if (autoTracking){
            internal.m_observedModel.modelChanged.disconnect(onModelChanged);
        }

        internal.m_observedModel = null;
        resetUndo();
    }


    function modelIsRegistered()
    {
        return internal.m_observedModel != null;
    }


    function isTransaction(){
        return internal.m_isBlocked;
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

        internal.m_beginStateModel = internal.m_observedModel.toJson();

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

        if (internal.m_beginStateModel != ""){
            internal.m_undoStack.push(internal.m_beginStateModel)
            internal.m_redoStack = []

            modelChanged();
        }
    }


    function setBlockingUpdateModel(blockingUpdateModel){
        internal.m_isBlocked = blockingUpdateModel;
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

        internal.m_defaultStateModel = model.copyMe();
    }


    // The first model in the stack is a copy of the observed model

    function getStandardModel()
    {
        return internal.m_defaultStateModel;
    }


    QtObject {
        id: internal;

        property bool m_isBlocked: false;

        property var m_observedModel: null;
        property string m_beginStateModel;
        property var m_defaultStateModel: null;

        property var m_undoStack: [];
        property var m_redoStack: [];
    }

    function onModelChanged(){
        if (internal.m_isBlocked){
            return;
        }

        undoRedoManager.makeChanges();

        internal.m_beginStateModel = internal.m_observedModel.toJson();
    }

    Shortcut {
        sequence: "Ctrl+Z";
        enabled: true;
        onActivated: {
            console.log("Ctrl+Z")
            let undoSteps = undoRedoManager.getAvailableUndoSteps();

            if (undoSteps > 0){
                undoRedoManager.commandHandle("Undo");
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+Shift+Z";
        enabled: true;
        onActivated: {
            console.log("Ctrl+Shift+Z")
            let redoSteps = undoRedoManager.getAvailableRedoSteps();
            if (redoSteps > 0){
                undoRedoManager.commandHandle("Redo");
            }
        }
    }
}


