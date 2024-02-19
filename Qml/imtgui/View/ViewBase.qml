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

    onReadOnlyChanged: {
        setReadOnly(readOnly);
    }

    onVisibleChanged: {
        console.log("onVisibleChanged", visible, model.toJSON());
        if (visible){
            viewBase.updateCommandsGui();
        }
        else{
            viewBase.clearCommandsGui();
        }
    }

    Connections {
        id: commandsConnections;

        function onCommandsModelChanged(){
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

            commandsController.commandsModelChanged.connect(internal.onCommandsModelChanged);
        }
    }

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

    function doUpdateModel()
    {
        console.log("View doUpdateModel");

        if (readOnly || internal.blockingUpdateModel || internal.blockingUpdateGui){
            return;
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;
    }

    function doUpdateGui()
    {
        console.log("View doUpdateGui");

        if (internal.blockingUpdateGui || internal.blockingUpdateModel){
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

        function onCommandsModelChanged(){
            console.log("onCommandsModelChanged");
            if (viewBase.visible){
                viewBase.updateCommandsGui();
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


