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
        if (commandsDelegateComp){
            if (commandsDelegate){
                commandsDelegate.destroy();
            }

            commandsDelegate = commandsDelegateComp.createObject(viewBase);
        }
    }

    onCommandsDelegateChanged: {
        if (viewBase.viewId !== ""){
            Events.unSubscribeAllFromSlot(item.commandHandle);
            Events.subscribeEvent(viewBase.viewId + "CommandActivated", item.commandHandle);
        }
    }

    onReadOnlyChanged: {
        setReadOnly(readOnly);
    }

    onVisibleChanged: {
        console.log("onVisibleChanged", viewBase, visible, model.toJSON());
        if (commandsController){
            if (visible){
                viewBase.updateCommandsGui();

//                if (internal.localizationChanged){
//                    internal.onLocalizationChanged();
//                }
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
        if (commandsController){
            Events.sendEvent("ClearCommandsGui", {"ViewId": viewBase.viewId});
        }
    }

    QtObject {
        id: internal;

        property bool localizationChanged: false;
        property TreeItemModel cachedCommandsModel: TreeItemModel {};
        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;

        property int countIncomingChanges: 0;

        Component.onDestruction: {
            console.log("internal onDestruction");
        }

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

        function onLocalizationChanged(language){
            console.log("onLocalizationChanged Events.events", Events.events["OnLocalizationChanged"]);

            console.log("ViewBase onLocalizationChanged", language);
//            if (!viewBase.visible){
//                localizationChanged = true;
//                return;
//            }

            if (viewBase.commandsController){
                internal.cachedCommandsModel.Clear();
                internal.cachedCommandsModel.Copy(viewBase.commandsController.commandsModel);
                viewBase.commandsController.updateModel();
            }

            localizationChanged = false
        }
    }

    MouseArea {
        id: blockArea;

        z: parent.z + 1;

        anchors.fill: parent;

        visible: viewBase.commandsController != null;
    }
}


