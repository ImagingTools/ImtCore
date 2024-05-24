import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: viewBase;

    property string viewId;
    property TreeItemModel model: TreeItemModel {}

    property Component commandsControllerComp: null;
    property CommandsController commandsController: null;

    property Component commandsDelegateComp: null;
    property ViewCommandsDelegateBase commandsDelegate: null;

    property bool readOnly: false;

    Component.onCompleted: {
        viewId = UuidGenerator.generateUUID();
        Events.subscribeEvent("OnLocalizationChanged", internal.onLocalizationChanged)
    }

    Component.onDestruction: {
        if (commandsDelegate){
            Events.unSubscribeEvent(viewId + "CommandActivated", commandsDelegate.commandHandle);
        }

        Events.unSubscribeEvent("OnLocalizationChanged", internal.onLocalizationChanged)
    }

    onCommandsControllerCompChanged: {
        if (commandsController){
            commandsController.destroy();
            commandsController = null
        }

        if (commandsControllerComp){
            commandsController = commandsControllerComp.createObject(viewBase);
        }
    }

    onCommandsDelegateCompChanged: {
        if (commandsDelegate){
            commandsDelegate.destroy();

            commandsDelegate = null;
        }

        if (commandsDelegateComp){
            commandsDelegate = commandsDelegateComp.createObject(viewBase);
        }
    }

    onCommandsDelegateChanged: {
        if (viewBase.viewId !== ""){
            Events.unSubscribeAllFromSlot(commandsDelegate.commandHandle);
            Events.subscribeEvent(viewBase.viewId + "CommandActivated", commandsDelegate.commandHandle);
        }
    }

    onReadOnlyChanged: {
        setReadOnly(readOnly);
    }

    onVisibleChanged: {
        if (commandsController){
            if (visible){
                if (internal.localizationChanged){
                    internal.updateCommandsAtferLocalizationChanged();
                }
            }
            else{
                viewBase.clearCommandsGui();
            }
        }
    }

    Connections {
        id: commandsConnections;

        function onCommandsModelChanged(){
            if (viewBase.commandsController){
                for (let i = 0; i < internal.cachedCommandsModel.GetItemsCount(); i++){
                    let id = internal.cachedCommandsModel.GetData("Id", i);
                    let isEnabled = internal.cachedCommandsModel.GetData("IsEnabled", i);

                    viewBase.commandsController.setCommandIsEnabled(id, isEnabled);
                }

                if (internal.localizationChanged){
                    viewBase.updateCommandsGui();

                    internal.localizationChanged = false;
                }
            }

            blockArea.visible = false;
        }
    }

    onViewIdChanged: {
        if (viewId !== ""){
            if (commandsDelegate){
                Events.unSubscribeAllFromSlot(commandsDelegate.commandHandle);
                Events.subscribeEvent(viewId + "CommandActivated", commandsDelegate.commandHandle);
            }
        }
    }

    function onModelChanged(){
        doUpdateGui();
    }

    function setReadOnly(readOnly){
    }

    onCommandsControllerChanged: {
        if (commandsController){
            commandsConnections.target = commandsController;
        }
    }

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

    // Update representation model from GUI
    function doUpdateModel()
    {
        if (readOnly || internal.blockingUpdateModel || internal.blockingUpdateGui){
            return;
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;
    }

    // Update GUI from representation model
    function doUpdateGui()
    {
        if (internal.blockingUpdateGui || internal.blockingUpdateModel){
            return;
        }

        internal.blockingUpdateModel = true;

        updateGui();

        internal.blockingUpdateModel = false;
    }

    function updateCommands(){
        if (!commandsController){
            console.error("Unable to update commands for view 'commandsController' is invalid")
            return;
        }

        commandsController.updateModel();
    }

    function setBlockingUpdateModel(value){
        internal.blockingUpdateModel = value;
    }

    function setBlockingUpdateGui(value){
        internal.blockingUpdateGui = value;
    }

    function updateCommandsGui(){
        if (commandsController && viewBase.viewId !== ""){
            if (commandsController.commandsModel.isReady){
                return;
            }

            Events.sendEvent("UpdateCommandsGui", {"Model": commandsController.commandsModel, "ViewId": viewBase.viewId});

//            Events.sendEvent("GetActiveCommandsViewId", function(activeViewId){
//                if (activeViewId !== viewBase.viewId){
//                }
//            });
        }
    }

    function clearCommandsGui(){
        if (commandsController){
            Events.sendEvent("ClearCommandsGui", {"ViewId": viewBase.viewId});
        }
    }

    function guiIsBlocked(){
        return internal.blockingUpdateGui;
    }

    QtObject {
        id: internal;

        property bool localizationChanged: false;
        property TreeItemModel cachedCommandsModel: TreeItemModel {};
        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;

        property int countIncomingChanges: 0;

        property bool startUpdateCommands: viewBase.visible && viewBase.commandsController != null && viewBase.commandsController.isReady;
        onStartUpdateCommandsChanged: {
            if (startUpdateCommands){
                viewBase.updateCommandsGui();
            }
        }

        onBlockingUpdateGuiChanged: {
            if (!blockingUpdateGui && countIncomingChanges > 0){
                countIncomingChanges = 0;
                viewBase.onModelChanged();
            }
        }

        function onLocalizationChanged(language){
            if (!viewBase.visible){
                localizationChanged = true;
                return;
            }

            updateCommandsAtferLocalizationChanged();
        }

        function updateCommandsAtferLocalizationChanged(){
            if (viewBase.commandsController){
                internal.cachedCommandsModel.Clear();
                internal.cachedCommandsModel.Copy(viewBase.commandsController.commandsModel);
                viewBase.commandsController.updateModel();
            }
        }
    }

    MouseArea {
        id: blockArea;

        z: parent.z + 1;

        anchors.fill: parent;

        visible: viewBase.commandsController != null;
    }
}


