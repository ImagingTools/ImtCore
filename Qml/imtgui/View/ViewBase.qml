import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: viewBase;

    property string viewId;

    property CommandsController commandsController: null;
    property TreeItemModel model: TreeItemModel {}
    property var commandsDelegate: null;
    property bool readOnly: false;

    Component.onCompleted: {
        viewId = UuidGenerator.generateUUID();
    }

    Component.onDestruction: {
        if (commandsDelegate){
            Events.unSubscribeEvent(viewId + "CommandActivated", commandsDelegate.commandHandle);
        }
    }

    onVisibleChanged: {
        console.log("onVisibleChanged", visible);
        if (visible){
            viewBase.updateCommandsGui();
        }
        else{
            viewBase.clearCommandsGui();
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

    onModelChanged: {
        console.log("ViewBase onModelChanged");
        if (internal.blockingUpdateGui || internal.blockingUpdateModel){
            internal.countIncomingChanges++;
            return;
        }

        doUpdateGui();
    }

    Connections {
        target: viewBase.commandsController;

        function onCommandsModelChanged(){
            console.log("onCommandsModelChanged");
            if (viewBase.visible){
                viewBase.updateCommandsGui();
            }
        }
    }

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

    function doUpdateModel()
    {
        if (readOnly || internal.blockingUpdateModel || internal.blockingUpdateGui){
            return;
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;
    }

    function doUpdateGui()
    {
        if (readOnly || internal.blockingUpdateGui || internal.blockingUpdateModel){
            return;
        }

        internal.blockingUpdateModel = true;

        updateGui();

        internal.blockingUpdateModel = false;
    }

    function updateCommands(){
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
            console.log("updateCommandsGui");
            Events.sendEvent("UpdateCommandsGui", {"Model": commandsController.commandsModel, "ViewId": viewBase.viewId});
        }
    }

    function clearCommandsGui(){
        Events.sendEvent("ClearCommandsGui", {"ViewId": viewBase.viewId});
    }

    QtObject {
        id: internal;

        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;

        property int countIncomingChanges: 0;

        onBlockingUpdateGuiChanged: {
            if (!blockingUpdateGui && countIncomingChanges > 0){
                countIncomingChanges = 0;
                viewBase.onModelChanged();
            }
        }
    }
}


