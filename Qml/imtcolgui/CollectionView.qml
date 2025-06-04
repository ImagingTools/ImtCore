import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

Item {
	id: root;
	
	property alias viewId: container.viewId;
	property alias collectionId: container.collectionId;
	
	property alias commandsViewComp: container.commandsViewComp;
	property alias commandsView: container.commandsView;
	
	property alias commandsControllerComp: container.commandsControllerComp;
	property alias commandsController: container.commandsController;
	
	property alias dataControllerComp: container.dataControllerComp;
	property alias dataController: container.dataController;
	
	property alias commandsDelegateComp: container.commandsDelegateComp;
	property alias commandsDelegate: container.commandsDelegate;
	
	property alias collectionFilter: container.collectionFilter;
	property alias documentCollectionFilter: container.documentCollectionFilter;
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
	property alias tableRowDelegate: container.tableRowDelegate;
	property alias scrollbarItem: container.scrollbarItem
	property alias scrollbarItemHoriz: container.scrollbarItemHoriz

	property bool visibleMetaInfo: false;
	property int metaInfoWidth: Style.sizeHintXXS;
	
	signal selectedIndexChanged(int index);
	signal elementsChanged();
	signal headersChanged();
	signal tableDecoratorChanged()
	
	/*!
		\qmlsignal selectionChanged(var selectedIds)
		Emitted whenever the selection changes.
		\param selectedIds The array of selected item IDs.
		\param selectedIndexes The array of selected item indexes.
	*/
	signal selectionChanged(var selectedIds, var selectedIndexes);
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
	
	onWidthChanged: {
		if (!visibleMetaInfo){
			return
		}
		
		collectionMetaInfo.width = width / 5 <= metaInfoWidth ? 0 : metaInfoWidth
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
	
	function removeElements(elementIds){
		container.removeElements(elementIds);
	}
	
	function removeElementSet(filter){
		container.removeElementSet(filter);
	}
	
	function restoreObjects(elementIds){
		if (container.dataController){
			container.dataController.restoreObjects(elementIds);
		}
	}
	
	function restoreObjectSet(filter){
		if (container.dataController){
			container.dataController.restoreObjectSet(filter);
		}
	}
	
	function setElementName(elementIndex, name){
		container.setElementName(elementIndex, name);
	}
	
	function setElementDescription(elementIndex, description){
		container.setElementDescription(elementIndex, description);
	}
	
	function setElementEnabled(elementId, enabled){
		if (container.dataController){
			container.dataController.setElementEnabled(elementId, enabled);
		}
	}
	
	function setMetaInfoModel(metaInfoModel){
		collectionMetaInfo.metaInfoModel = metaInfoModel;
	}
	
	function onEdit(id, index){
		if (container.commandsDelegate){
			container.commandsDelegate.commandHandle("Edit");
		}
	}
	
	function getSelectedIds(){
		return container.getSelectedIds()
	}
	
	function hasActiveFilter(){
		console.log("hasActiveFilter", collectionFilter.toJson())
		if (collectionFilter){
			return !collectionFilter.isEmpty()
		}
		
		return false
	}

	Connections {
		target: container.commandsController;
		
		function onCommandsChanged(){
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

		function onTableDecoratorChanged(){
			root.tableDecoratorChanged()
		}
		
		function onSelectionChanged(ids, indexes){
			root.selectionChanged(ids, indexes);
			
			collectionMetaInfo.contentVisible = ids.length === 1;
			additionalInformation.visible = root.visibleMetaInfo && ids.length === 0;
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
				internal.isReady = true;
				container.doUpdateGui();
			}

			function onNotificationModelChanged(){
				if (!container.dataController){
					return;
				}
				
				let notificationModel = container.dataController.notificationModel;
				if (!notificationModel){
					container.pagination.countAllElements = 0
					container.pagination.pagesSize = 1
					return
				}
				
				if (notificationModel.containsKey("totalCount")){
					let totalCount = notificationModel.getData("totalCount")
					container.pagination.countAllElements = totalCount;
				}
				
				if (notificationModel.containsKey("pagesCount")){
					let pagesCount = notificationModel.getData("pagesCount")
					container.pagination.pagesSize = pagesCount;
				}
			}
			
			function onFilterableHeadersModelChanged(){
				if (!container.dataController){
					return;
				}
				
				let filteringIds = []
				let filteringModel = container.dataController.filterableHeadersModel;
				for (let i = 0; i < filteringModel.getItemsCount(); i++){
					let infoId = filteringModel.getData("id", i);
					filteringIds.push(infoId)
				}
				
				container.collectionFilter.setFilteringInfoIds(filteringIds);
			}
		}
		
		QtObject {
			id: internal;
			
			property bool isReady: false;

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
		
		function doUpdateGui(){
			if (!internal.isReady){
				return;
			}
			
			let count = -1;
			let offset = 0;
			
			if (hasPagination){
				count = pagination.countElements;
				offset = pagination.currentIndex * count;
			}
			
			if (dataController){
				let documentFilter = null
				if (!documentCollectionFilter.isEmpty()){
					documentFilter = documentCollectionFilter
				}

				dataController.updateElements(count, offset, collectionFilter, documentFilter);
			}
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
		
		function removeElements(elementIds){
			if (container.dataController){
				return container.dataController.removeElements(elementIds);
			}
		}
		
		function removeElementSet(filter){
			if (container.dataController){
				return container.dataController.removeElementSet(filter);
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
		
		width: visible ? root.metaInfoWidth : 0;
		height: parent.height;
		
		visible: root.visibleMetaInfo;
	}
	
	Rectangle {
		id: additionalInformation;
		
		anchors.fill: collectionMetaInfo;
		anchors.margins: Style.marginM;
		
		color: collectionMetaInfo.color;
		
		visible: root.visibleMetaInfo;
		
		Text {
			id: valueText;
			
			anchors.fill: parent;
			
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
			
			wrapMode: Text.WordWrap;
			color: Style.textColor;
			
			text: qsTr("Please select an item for showing additional informations");
		}
	}
}


