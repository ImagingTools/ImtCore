import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: viewBase;

	property string updateCommandsGuiEventCommandId;
	property string clearCommandsGuiEventCommandId;

	property string viewId; // uuid unic identifier
    property var model: null;

    property Component commandsControllerComp: null;
    property CommandsController commandsController: null;

	property Component commandsDelegateComp: Component {
		ViewCommandsDelegateBase {
			view: viewBase;
		}
	}

    property ViewCommandsDelegateBase commandsDelegate: null;

    property alias commandsView: headerViewLoader.item;
    property alias commandsViewComp: headerViewLoader.sourceComponent;

    property bool readOnly: false;

    signal commandsModelChanged(var commandsModel)
	signal commandActivated(string commandId);

	Connections {
		target: viewBase.commandsDelegate
		function onCommandActivated(commandId){
			viewBase.commandActivated(commandId)
		}

	}

    Component {
        id: commandsDecoratorComp;

        CommandsPanel {
            commandId: viewBase.viewId;
            onCommandActivated: {
                if (viewBase.commandsDelegate){
                    viewBase.commandsDelegate.commandHandle(commandId);
                }
            }
        }
    }

	function setAlertPanel(alertPanelComp){
		alertPanel.sourceComponent = alertPanelComp;
	}

	Loader {
		id: alertPanel;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: visible ? 40: 0;
		visible: alertPanel.item != null && alertPanel.item !== undefined;
		objectName: "ViewBase";
	}

    Item {
        id: headerViewItem;
		anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: visible ? 30 + 2 * Style.size_mainMargin : 0;
        objectName: "ViewBase";
        visible: headerViewLoader.item && viewBase.commandsController != null;

        Loader {
            id: headerViewLoader;
            anchors.verticalCenter: parent.verticalCenter;
            width: parent.width;
            sourceComponent: commandsDecoratorComp;
            onLoaded: {
                if (viewBase.commandsController){
                    viewBase.commandsController.commandsView = item;
                }
            }
        }
    }

    Rectangle {
        id: separator;
        anchors.top: headerViewItem.bottom;
        width: parent.width;
        height: visible ? 1 : 0;
        color: Style.borderColor;
        visible: headerViewLoader.item && viewBase.commandsController != null;
        objectName: "ViewBase";
    }

    Rectangle {
        id: viewContent;
        anchors.top: separator.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        objectName: "ViewBase";
        color: Style.backgroundColor2;
    }

    Component.onCompleted: {
        viewId = UuidGenerator.generateUUID();

        for (let i = 0; i < viewBase.children.length; i++){
            if (viewBase.children[i].objectName !== "ViewBase"){
                let element = viewBase.children[i];
                element.parent = viewContent;
                i--;
            }
        }

		if (commandsControllerComp){
			commandsController = commandsControllerComp.createObject(viewBase);
		}

		if (commandsDelegateComp){
			commandsDelegate = commandsDelegateComp.createObject(viewBase);
		}
    }

    Component.onDestruction: {
        if (commandsDelegate){
            Events.unSubscribeEvent(viewId + "CommandActivated", commandsDelegate.commandHandle);
        }

        internal.isDestroyed = true;
    }

    onCommandsControllerChanged: {
        if (commandsController){
            commandsController.commandsView = viewBase.commandsView;
        }
    }

    onCommandsDelegateChanged: {
        if (viewBase.viewId !== ""){
            Events.unSubscribeAllFromSlot(commandsDelegate.commandHandle);
            Events.subscribeEvent(viewBase.viewId + "CommandActivated", commandsDelegate.commandHandle);
        }

		if (commandsDelegate){
			commandsDelegate.view = viewBase;
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
        }
    }

    Connections {
        id: commandsConnections;
		target: viewBase.commandsController;

        function onCommandsModelChanged(){
            if (viewBase.commandsController){
                for (let i = 0; i < internal.cachedCommandsModel.getItemsCount(); i++){
                    let subElements = internal.cachedCommandsModel.getData("SubElements", i);
                    if (subElements){
                        for (let j = 0; j < subElements.getItemsCount(); j++){
                            let id = subElements.getData("Id", j);
                            let isEnabled = subElements.getData("IsEnabled", j);
                            viewBase.commandsController.setCommandIsEnabled(id, isEnabled);
                        }
                    }
                }

                if (internal.localizationChanged){
                    viewBase.updateCommandsGui();

                    internal.localizationChanged = false;
                }
				console.log("ViewBase onCommandsModelChanged");
				viewBase.commandsModelChanged(viewBase.commandsController.commandsModel)
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

    // for override
    function updateGui(){}

    // for override
    function updateModel(){}

    // Update representation model from GUI
    function doUpdateModel()
    {
        if (!model){
            return;
        }

        if (readOnly || internal.blockingUpdateModel || internal.blockingUpdateGui){
            return;
        }

        console.debug("doUpdateModel", model);

        if (model.beginChanges !== undefined){
            model.beginChanges();
        }

        internal.blockingUpdateGui = true;

        updateModel();

        internal.blockingUpdateGui = false;

        if (model.endChanges !== undefined){
            model.endChanges();
        }
    }

    // Update GUI from representation model
    function doUpdateGui()
    {
        if (!model){
            return;
        }

        if (internal.blockingUpdateGui || internal.blockingUpdateModel){
            return;
        }

        console.debug("doUpdateGui", model);

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
        if (commandsView && commandsController){
            commandsView.setCommandsModel({"Model": commandsController.commandsModel, "ViewId": viewBase.viewId});
        }
    }

    function clearCommandsGui(){
        if (commandsView && commandsController){
            commandsView.clearModel({"ViewId": viewBase.viewId});
        }
    }

    function guiIsBlocked(){
        return internal.blockingUpdateGui;
    }

	LocalizationEvent {
		onLocalizationChanged: {
			internal.onLocalizationChanged(langId)
		}
	}

    QtObject {
        id: internal;

        property bool localizationChanged: false;
        property TreeItemModel cachedCommandsModel: TreeItemModel {};
        property bool blockingUpdateGui: false;
        property bool blockingUpdateModel: false;
        property bool isDestroyed: false;

        property bool startUpdateCommands: viewBase.visible && viewBase.commandsController && viewBase.commandsController.isReady;
        onStartUpdateCommandsChanged: {
            if (internal.isDestroyed){
                return;
            }

            if (viewBase.commandsController){
                if (startUpdateCommands){
                    viewBase.updateCommandsGui();
                }
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
                internal.cachedCommandsModel.clear();
                internal.cachedCommandsModel.copy(viewBase.commandsController.commandsModel);
                viewBase.commandsController.updateModel();
            }
        }
    }

    MouseArea {
        id: blockArea;
        z: parent.z + 2;
        anchors.fill: parent;
        visible: viewBase.commandsController != null;
        objectName: "ViewBase";
    }
}


