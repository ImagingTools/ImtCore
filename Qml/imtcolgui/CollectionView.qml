import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

Item {
	id: root;
	
	property alias viewId: container.viewId;
	property alias collectionId: container.collectionId;
	
	property alias commandsViewComp: container.commandsViewComp;
	property alias commandsView: container.commandsView;
	
	property alias updateCommandsGuiEventCommandId: container.updateCommandsGuiEventCommandId;
	property alias clearCommandsGuiEventCommandId: container.clearCommandsGuiEventCommandId;
	
	property alias commandsControllerComp: container.commandsControllerComp;
	property alias commandsController: container.commandsController;
	
	property alias dataControllerComp: container.dataControllerComp;
	property alias dataController: container.dataController;
	
	property alias commandsDelegateComp: container.commandsDelegateComp;
	property alias commandsDelegate: container.commandsDelegate;
	
	property alias collectionFilter: container.collectionFilter;
	property alias table: container.table;
	
	property alias readOnly: container.readOnly;
	property alias filterMenu: container.filterMenu;
	property alias filterMenuVisible: container.filterMenuVisible;
	property alias hasFilter: container.hasFilter;
	property alias hasSort: container.hasSort;
	property alias hasPagination: container.hasPagination;
	property alias elementsCount: container.elementsCount;
	property alias pagination: container.pagination;
	property alias metaInfoView: collectionMetaInfo;
	
	property bool visibleMetaInfo: false;
	
	signal selectedIndexChanged(int index);
	signal elementsChanged();
	signal headersChanged();
	signal selectionChanged(var selection);
	signal checkedItemsChanged();
	signal rightButtonMouseClicked(int mouseX, int mouseY);
	signal tableViewParamsAccepted();
	
	LocalizationEvent {
		onLocalizationChanged: {
			if (root.dataController){
				root.dataController.updateModel();
			}
		}
	}
	
	Keys.onPressed: {
		if (event.key === Qt.Key_Delete){
			container.commandsDelegate.commandHandle("Remove");
		}
	}
	
	function doubleClicked(id, index){
		container.doubleClicked(id, index)
	}
	
	function setAlertPanel(alertPanelComp){
		container.setAlertPanel(alertPanelComp)
	}
	
	function doUpdateGui(){
		container.doUpdateGui();
	}
	
	function doUpdateModel(){
		container.doUpdateModel();
	}
	
	function removeElement(elementIndex){
		container.removeElement(elementIndex);
	}
	
	function setElementName(elementIndex, name){
		container.setElementName(elementIndex, name);
	}
	
	function setElementDescription(elementIndex, description){
		container.setElementDescription(elementIndex, description);
	}
	
	function setMetaInfoModel(metaInfoModel){
		collectionMetaInfo.metaInfoModel = metaInfoModel;
	}
	
	function onEdit(id, index){
		if (container.commandsDelegate){
			container.commandsDelegate.commandHandle("Edit");
		}
	}
	
	function onFilterChanged(filterId, filterValue){
		container.collectionFilter.setTextFilter(filterValue);
		
		container.doUpdateGui();
	}
	
	Connections {
		target: container.commandsController;
		
		function onCommandsModelChanged(){
			if (root.commandsDelegate){
				root.commandsDelegate.setupContextMenu();
			}
		}
	}
	
	Connections {
		target: container;
		
		function onSelectedIndexChanged(index){
			root.selectedIndexChanged(index);
		}
		
		function onElementsChanged(){
			root.elementsChanged();
		}
		
		function onHeadersChanged(){
			root.headersChanged();
		}
		
		function onSelectionChanged(selection){
			root.selectionChanged(selection);
			
			collectionMetaInfo.contentVisible = selection.length === 1;
			additionalInformation.visible = root.visibleMetaInfo && selection.length === 0;
		}
		
		function onRightButtonMouseClicked(mouseX, mouseY){
			root.rightButtonMouseClicked(mouseX, mouseY)
		}
	}
	
	CollectionViewBase {
		id: container;
		
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: collectionMetaInfo.left;
		
		property Component dataControllerComp;
		property var dataController: null;
		
		onTableViewParamsAccepted: {
			root.tableViewParamsAccepted();
		}
		
		commandsDelegateComp: Component {
			CollectionViewCommandsDelegateBase {
				collectionView: container;
			}
		}
		
		onCheckedItemsChanged: {
			root.checkedItemsChanged();
		}
		
		Connections {
			target: container.dataController;
			
			function onBeginUpdate(){
				container.loading.start();
			}
			
			function onEndUpdate(){
				container.loading.stop();
			}
			
			function onHeadersModelChanged(){
				container.doUpdateGui();
			}
			
			function onNotificationModelChanged(){
				if (!container.dataController){
					return;
				}
				
				let notificationModel = container.dataController.notificationModel;
				if (notificationModel.containsKey("TotalCount")){
					let totalCount = notificationModel.getData("TotalCount")
					
					container.pagination.countAllElements = totalCount;
				}
				
				if (notificationModel.containsKey("PagesCount")){
					let pagesCount = notificationModel.getData("PagesCount")
					
					container.pagination.pagesSize = pagesCount;
				}
			}
			
			// function onFilterableHeadersModelChanged(){
			// 	if (!container.dataController){
			// 		return;
			// 	}

			// 	let filteringIds = []
			// 	let filteringModel = container.dataController.filterableHeadersModel;
			// 	for (let i = 0; i < filteringModel.getItemsCount(); i++){
			// 		let infoId = filteringModel.getData("Id", i);
			// 		filteringIds.push(infoId)
			// 	}

			// 	container.collectionFilter.setFilteringInfoIds(filteringIds);
			// }

			function onFilterableHeadersModelChanged(){
				if (!container.dataController){
					return;
				}

				container.collectionFilter.setFilteringInfoIds(container.dataController.filterableHeadersModel);
			}
		}
		
		QtObject {
			id: internal;
			
			property bool guiUpdateRequired: false
			
			function filterMenuActivate(){
				if (container.hasFilter){
					container.filterMenuVisible = !container.filterMenuVisible;
				}
			}
		}
		
		Component.onCompleted: {
			Events.subscribeEvent("FilterActivated", internal.filterMenuActivate);
			
			if (dataControllerComp){
				dataController = dataControllerComp.createObject(container);
			}
		}
		
		Component.onDestruction: {
			Events.unSubscribeEvent("FilterActivated", internal.filterMenuActivate);
		}
		
		onDataControllerChanged: {
			if (container.dataController){
				bindHeaders.target = container.table;
				bindElements.target = container.table;
			}
		}
		
		onRightButtonMouseClicked: {
			openPopupMenu(mouseX, mouseY);
		}
		
		onDoubleClicked: {
			root.onEdit(id, index)
		}
		
		onFilterChanged: {
			root.onFilterChanged(filterId, filterValue);
		}
		
		Binding {
			id: bindHeaders;
			property: "headers";
			
			value: container.dataController.headersModel;
		}
		
		Binding {
			id: bindElements;
			property: "elements";
			value: container.dataController.elementsModel;
		}
		
		onVisibleChanged: {
			if (container.visible){
				if (internal.guiUpdateRequired){
					internal.guiUpdateRequired = false;
					container.doUpdateGui();
				}
			}
		}
		
		function doUpdateGui(){
			if (!container.visible){
				internal.guiUpdateRequired = true;
				
				return;
			}
			
			console.log("doUpdateGui");
			
			let count = -1;
			let offset = 0;
			
			if (container.hasPagination){
				count = pagination.countElements;
				offset = pagination.currentIndex * count;
			}
			
			if (container.dataController){
				// container.dataController.updateElements(count, offset, container.collectionFilter);
				container.dataController.updateElements(count, offset, container.collectionFilter.filterModel);
			}
			
			container.table.resetSelection();
		}
		
		function openPopupMenu(x, y){
			if (container.commandsDelegate){
				let contextMenuModel = container.commandsDelegate.getContextMenuModel();
				let count = contextMenuModel.getItemsCount();
				if (count > 0){
					let offset = 30 * count;
					ModalDialogManager.openDialog(popupMenuDialog, {"x": x, "y": y - offset, "model": contextMenuModel});
				}
			}
		}
		
		function removeElement(elementIndex){
			if (container.dataController){
				return container.dataController.removeElement(elementIndex);
			}
		}
		
		function setElementName(elementIndex, name){
			if (container.dataController){
				return container.dataController.setElementName(elementIndex, name);
			}
		}
		
		function setElementDescription(elementIndex, description){
			if (container.dataController){
				return container.dataController.setElementDescription(elementIndex, description);
			}
		}
		
		Component {
			id: popupMenuDialog;
			
			PopupMenuDialog {
				onFinished: {
					if (commandId !== ""){
						container.commandsDelegate.commandHandle(commandId);
					}
				}
			}
		}
	}
	
	MetaInfo {
		id: collectionMetaInfo;
		
		anchors.top: parent.top;
		anchors.right: parent.right;
		
		width: visible ? 200 : 0;
		height: parent.height;
		
		visible: root.visibleMetaInfo;
	}
	
	Rectangle {
		id: additionalInformation;
		
		anchors.fill: collectionMetaInfo;
		anchors.margins: Style.sizeMainMargin;
		
		color: collectionMetaInfo.color;
		
		visible: root.visibleMetaInfo;
		
		Text {
			id: valueText;
			
			anchors.fill: parent;
			
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeXSmall;
			
			wrapMode: Text.WordWrap;
			color: Style.textColor;
			
			text: qsTr("Please select an item for showing additional informations");
		}
	}
}


