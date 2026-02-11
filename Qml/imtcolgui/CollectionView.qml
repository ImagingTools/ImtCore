import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

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

	property alias contentColor: container.contentColor;

	property bool visibleMetaInfo: false;
	property bool headerRightClickEnabled: true;
	property bool commandsPanelVisible: true
	property bool loadingDataAfterHeadersReceived: true
	property bool backgroundUpdatesEnabled: false

	property alias canResetFilters: container.canResetFilters;
	property int metaInfoWidth: Style.sizeHintXXS;
	property alias contentHeight: container.contentHeight

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

	signal commandActivated(string commandId);
	
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

	Shortcut {
		sequence: "F2"
		enabled: root.visible
		onActivated: {
			container.commandsDelegate.commandHandle("Rename")
		}
	}
	
	onWidthChanged: {
		if (!visibleMetaInfo){
			return
		}
		
		collectionMetaInfo.width = width / 5 <= metaInfoWidth ? 0 : metaInfoWidth
	}

	Component.onCompleted: {
		registerFilter("ComplexFilter", root.collectionFilter)
		registerFilter("DocumentFilter", root.documentCollectionFilter)
	}

	onCollectionFilterChanged: {
		registerFilter("ComplexFilter", root.collectionFilter)
	}

	onDocumentCollectionFilterChanged: {
		registerFilter("DocumentFilter", root.documentCollectionFilter)
	}

	function registerFilter(filterId, filterModel){
		filterMenu.registeredFilters[filterId] = filterModel
	}

	function setFilterIsEnabled(filterId, enabled){
		filterMenu.setFilterIsEnabled(filterId, enabled)
	}

	function clearAllFilters(beQuiet){
		filterMenu.clearAllFilters(beQuiet)
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

	function duplicateElements(elementIds, name){
		if (container.dataController){
			container.dataController.duplicateElements(elementIds, name)
		}
	}
	
	function setElementName(elementId, name){
		container.setElementName(elementId, name);
	}
	
	function setElementDescription(elementId, description){
		container.setElementDescription(elementId, description);
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
		return container.hasActiveFilter()
	}
	
	function registerFieldFilterDelegate(filterId, filterDelegateComp){
		container.registerFieldFilterDelegate(filterId, filterDelegateComp)
	}

	function unregisterFieldFilterDelegate(filterId){
		container.filterMenu.removeFieldFilterDelegate(filterId)
	}

	function setFilterDependency(filterId, dependsOnFilterId){
		container.setFilterDependency(filterId, dependsOnFilterId)
	}

	function registerMetaInfoViewDelegate(metaInfoId, viewComp){
		metaInfoView.registerViewDelegate(metaInfoId, viewComp)
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
			
			if (root.dataController){
				if (root.visibleMetaInfo && ids.length === 1){
					collectionMetaInfo.startLoading()
					root.dataController.getObjectMetaInfo(ids[0])
				}
			}

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

		headerRightClickEnabled: root.headerRightClickEnabled;
		commandsPanelVisible: root.commandsPanelVisible;
		
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
		onCommandActivated: {
			root.commandActivated(commandId)
		}
		Connections {
			target: container.dataController;
			
			function onBeginUpdate(){
				container.loading.start();
			}
			
			function onEndUpdate(){
				container.loading.stop();

				if(root.visibleMetaInfo ){
					let ids = container.selectionManager.selectedIds
					if (root.dataController){
						if (ids.length === 1){
							collectionMetaInfo.startLoading()
							root.dataController.getObjectMetaInfo(ids[0])
						}
					}
					collectionMetaInfo.contentVisible = ids.length === 1;
					additionalInformation.visible = root.visibleMetaInfo && ids.length === 0;
				}
			}

			function onElementsRemoved(elementIds){
				container.selectionManager.deselect(elementIds)
				container.table.resetSelection()
			}

			function onElementSetRemoved(){
				container.selectionManager.clear()
				container.table.resetSelection()
			}

			function onObjectMetaInfoReceived(metaInfoData){
				collectionMetaInfo.elementMetaInfo = metaInfoData
				collectionMetaInfo.stopLoading()
			}
			
			function onHeadersReceived(headers){
				container.table.headers = headers

				let filteringIds = []
				for (let i = 0; i < headers.getItemsCount(); ++i){
					let headerInfo = headers.get(i).item
					if (headerInfo.m_filterable){
						filteringIds.push(headerInfo.m_id)
					}
				}

				container.collectionFilter.setFilteringInfoIds(filteringIds)

				if (root.loadingDataAfterHeadersReceived){
					container.doUpdateGui()
				}
			}

			function onElementsReceived(elements){
				container.table.elements = elements
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
		}
		
		QtObject {
			id: internal;
			property bool updateGuiRequired: false
		}
		
		Component.onCompleted: {
			if (dataControllerComp){
				dataController = dataControllerComp.createObject(container);
			}
		}

		onRightButtonMouseClicked: {
			openPopupMenu(mouseX, mouseY);
		}
		
		onDoubleClicked: {
			root.onEdit(id, index)
		}

		onVisibleChanged: {
			if (visible && internal.updateGuiRequired){
				doUpdateGui()
				internal.updateGuiRequired = false
			}
		}

		ParamsSetController {
			id: paramsSetController
		}

		Component {
			id: paramsSetComp
			ParamsSet {
			}
		}

		function doUpdateGui(){
			if (!root.backgroundUpdatesEnabled && !visible){
				internal.updateGuiRequired = true
				return
			}

			let count = -1;
			let offset = 0;

			if (hasPagination){
				count = pagination.countElements;
				offset = pagination.currentIndex * count;
			}

			if (dataController){
				let documentFilter = null
				if (documentCollectionFilter && !documentCollectionFilter.isEmpty()){
					documentFilter = documentCollectionFilter
				}

				let paramsSet = paramsSetComp.createObject(this)
				paramsSetController.paramsSet = paramsSet

				for (let filterKey in root.filterMenu.registeredFilters){
					if (root.filterMenu.enabledFilters[filterKey]){
						let filterModel = root.filterMenu.registeredFilters[filterKey]
						if (filterModel){
							paramsSetController.addParam(filterKey, filterKey, "", "", filterModel.toJson())
						}
					}
				}

				dataController.updateElements(count, offset, paramsSet);
				paramsSetController.paramsSet.destroy()
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
		
		function setElementName(elementId, name){
			if (container.dataController){
				container.dataController.setElementName(elementId, name);
			}
		}
		
		function setElementDescription(elementId, description){
			if (container.dataController){
				container.dataController.setElementDescription(elementId, description);
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


