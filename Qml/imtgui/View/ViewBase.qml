import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: viewBase;

	property string viewTypeId
	property string viewId
	property var model: null
	
	property Component commandsControllerComp: null;
	property CommandsController commandsController: null;
	property bool commandsRequested: false;
	
	property Component commandsDelegateComp: Component {
		ViewCommandsDelegateBase {
			view: viewBase;
		}
	}
	
	property ViewCommandsDelegateBase commandsDelegate: null;
	
	property alias commandsView: headerViewLoader.item;
	property alias commandsViewComp: headerViewLoader.sourceComponent;
	
	property bool readOnly: false;

	property string contentColor: Style.baseColor

	property bool commandsPanelVisible: true
	property bool commandsSeparatorVisible: true
	property int commandsPanelHeight: Style.controlHeightM + 2 * Style.marginM

	property real viewContentY: viewContent.y
	property real viewContentRightMargin: 0

	property bool hasButtonPanel: false
	property BaseModel buttonPanelModel: null

	signal commandsModelChanged(var commandsModel)
	signal commandActivated(string commandId)
	signal commandActivatedWithParams(string commandId, var params)
	signal modelDataChanged(var view, var model)
	signal guiUpdated(var view, var model)
	signal guiVisibleChanged(var view, bool visible)

	onModelChanged: {
		doUpdateGui()
	}

onCommandsModelChanged: {
		viewBase.buttonPanelModel = null
		if (!hasButtonPanel || !commandsModel){
			return
		}
		for (let i = 0; i < commandsModel.m_elements.count; i++){
			let elementGroup = commandsModel.m_elements.get(i).item
			if (elementGroup.m_alignment === -1){
				viewBase.buttonPanelModel = elementGroup.m_subElements
				break
			}
		}
	}

	Connections {
		target: viewBase.model
		function onModelChanged(){
			if (viewBase.internal__.blockingUpdateModel){
				return
			}

			viewBase.modelDataChanged(viewBase, viewBase.model)
		}
	}
	
	Connections {
		target: viewBase.commandsDelegate
		function onCommandActivated(commandId){
			viewBase.commandActivated(commandId)
		}
	}
	
	Component {
		id: commandsDecoratorComp;
		
		CommandsPanel {
			onCommandActivated: {
				if (viewBase.commandsDelegate){
					viewBase.commandsDelegate.commandHandle(commandId, params);
				}
			}
			
			onCommandsReady: {
				viewBase.commandsModelChanged(commandsModel)
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
		height: visible ? Style.controlHeightL: 0;
		visible: alertPanel.item != null && alertPanel.item !== undefined;
		objectName: "ViewBase";
	}
	
	Item {
		id: headerViewItem;
		anchors.top: alertPanel.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: visibleState ? viewBase.commandsPanelHeight : 0;
		visible: visibleState

		objectName: "ViewBase";
		property bool visibleState: !viewBase.commandsPanelVisible ? false : headerViewLoader.item && viewBase.commandsController != null;

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
		height: visibleState ? 1 : 0;
		color: Style.borderColor;
		opacity: 0.5
		visible: visibleState
		property bool visibleState: !viewBase.commandsPanelVisible ? false : headerViewLoader.item && viewBase.commandsController != null;

		objectName: "ViewBase";
	}
	
	Rectangle{
		id: buttonPanel
		anchors.left: parent.left
		anchors.top: separator.bottom
		anchors.bottom: parent.bottom
		width: viewBase.hasButtonPanel ? 0.5 * Style.sizeHintBXS : 0
		clip: true

		objectName: "ViewBase";


		Rectangle{

			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.right: parent.right

			width: 1

			color: Style.borderColor;
			opacity: 0.5
		}

		ListView{
			id: buttonsList

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter

			width: parent.width - Style.marginM
			height: Math.min(parent.height - 2*Style.marginM, contentHeight)

			boundsBehavior: Flickable.StopAtBounds

			model: viewBase.buttonPanelModel

			delegate: Item{
				anchors.horizontalCenter: parent.horizontalCenter
				width: buttonsList.width
				height: visible ? width: 0

				visible: model.item.m_visible

				Item {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width - Style.marginM
					height: width

					Image {
						id: imageName

						width: parent.width
						height: parent.height
						sourceSize.width: width
						sourceSize.height: height
						source: "../../../../" + Style.getIconPath(model.item.m_elementItemPath, Icon.State.On, Icon.Mode.Normal);
					}

					MouseArea{
						id: ma
						anchors.fill: parent

						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							let params = ({})
							params.target = this
							params.x = mouse.x
							params.y = mouse.y
							viewBase.commandActivated(model.item.m_elementId)
							viewBase.commandActivatedWithParams(model.item.m_elementId, params)
						}
					}
					TooltipArea {
						anchors.fill: parent;

						mouseArea: ma;
						text: model.item.m_elementName
					}
				}
			}
		}
	}

	Rectangle {
		id: viewContent;
		anchors.top: separator.bottom;
		anchors.left: buttonPanel.right;
		anchors.right: parent.right;
		anchors.rightMargin: viewBase.viewContentRightMargin
		anchors.bottom: parent.bottom;
		objectName: "ViewBase";
		color: viewBase.contentColor;
	}
	
	Component.onCompleted: {
		for (let i = 0; i < viewBase.children.length; i++){
			if (viewBase.children[i].objectName !== "ViewBase"){
				let element = viewBase.children[i];
				element.parent = viewContent;
				i--;
			}
		}
		
		if (commandsControllerComp){
			commandsController = commandsControllerComp.createObject(viewBase);
			commandsController.view = viewBase;
			requestCommands()
		}
		
		if (commandsDelegateComp){
			commandsDelegate = commandsDelegateComp.createObject(viewBase);
		}
	}
	
	onCommandsDelegateChanged: {
		if (commandsDelegate){
			commandsDelegate.view = viewBase;
		}
	}
	
	onReadOnlyChanged: {
		setReadOnly(readOnly);
	}
	
	onVisibleChanged: {
		requestCommands()

		guiVisibleChanged(viewBase, visible)
	}
	
	Connections {
		target: viewBase.commandsController;
		function onTypeIdChanged(){
			viewBase.requestCommands()
		}

		function onCommandsReceived(typeId, commands){
			viewBase.commandsView.commandsModel = commands
			
			if (viewBase.visible || viewBase.internal__.localizationChanged){
				viewBase.internal__.updateStatesFromCache()
				viewBase.internal__.localizationChanged = false
			}
		}
	}

	function requestCommands(){
		if (!commandsController || !visible || commandsRequested || commandsController.typeId === ""){
			return
		}

		commandsRequested = true;
		commandsController.getCommands()
	}
	
	ApplicationEvents {
		onLocalizationChanged: {
			viewBase.internal__.cacheCommandsState();
			
			if (viewBase.visible && viewBase.commandsController){
				viewBase.commandsController.getCommands();
			}
			else{
				viewBase.internal__.localizationChanged = true
			}
		}
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
		if (!model || !viewBase.internal__){
			return;
		}
		
		if (readOnly || viewBase.internal__.blockingUpdateModel || viewBase.internal__.blockingUpdateGui){
			return;
		}
		
		console.debug("doUpdateModel", model);
		
		if (model.beginChanges !== undefined){
			model.beginChanges();
		}
		
		viewBase.internal__.blockingUpdateGui = true;
		
		updateModel();
		
		viewBase.internal__.blockingUpdateGui = false;
		
		if (model.endChanges !== undefined){
			model.endChanges();
		}
	}

	// Update GUI from representation model
	function doUpdateGui()
	{
		console.debug("doUpdateGui");
		if (!model || !viewBase.internal__){
			return;
		}
		
		if (viewBase.internal__.blockingUpdateGui || viewBase.internal__.blockingUpdateModel){
			return;
		}
		
		viewBase.internal__.blockingUpdateModel = true;
		
		updateGui();
		
		viewBase.internal__.blockingUpdateModel = false;

		guiUpdated(this, model)
	}
	
	function setBlockingUpdateModel(value){
		viewBase.internal__.blockingUpdateModel = value;
	}
	
	function setBlockingUpdateGui(value){
		if (viewBase.internal__){
			viewBase.internal__.blockingUpdateGui = value;
		}
	}
	
	function guiIsBlocked(){
		if (!viewBase.internal__){
			return false
		}

		return viewBase.internal__.blockingUpdateGui;
	}
	
	property QtObject internal__: QtObject {
		property bool localizationChanged: false;
		property bool blockingUpdateGui: false;
		property bool blockingUpdateModel: false;

		property var commandStates: ({})
		
		function cacheCommandsState(){
			commandStates = {}
			if (viewBase.commandsController){
				let commandIds = viewBase.commandsController.getCommandIds()
				for (let i = 0; i < commandIds.length; i++){
					let commandId = commandIds[i]
					commandStates[commandId] = viewBase.commandsController.commandIsEnabled(commandId)
				}
			}
		}
		
		function updateStatesFromCache(){
			if (viewBase.commandsController){
				let keys = Object.keys(commandStates)
				for (let i = 0; i < keys.length; i++){
					let commandId = keys[i]
					let state = commandStates[commandId]
					
					viewBase.commandsController.setCommandIsEnabled(commandId, state)
				}
			}
		}
	}
}


