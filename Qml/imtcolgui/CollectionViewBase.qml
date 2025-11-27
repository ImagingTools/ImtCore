import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

ViewBase {
	id: collectionViewBaseContainer;
	
	clip: true;
	
	property string collectionId;
	
	property alias table: tableInternal;
	property bool hasPagination: true;
	property bool hasFilter: true;
	
	property bool hasSort: true;
	property bool canResetFilters: true;
	property bool headerRightClickEnabled: true;
	property alias isMultiCheckable: tableInternal.isMultiCheckable;
	
	property alias filterMenu: filterMenu_;
	property alias loading: loading_;
	property bool filterMenuVisible: true
	property alias pagination: pagination_;
	property alias elementsCount: tableInternal.elementsCount;
	property alias tableRowDelegate: tableInternal.rowDelegate
	property alias scrollbarItem: tableInternal.scrollbarItem
	property alias scrollbarItemHoriz: tableInternal.scrollbarItemHoriz

	property CollectionFilter collectionFilter: CollectionFilter {}
	property DocCollectionFilter documentCollectionFilter: DocCollectionFilter {}
	property IdSelectionManager selectionManager: IdSelectionManager {}

	property Component dataControllerComp: Component{CollectionDataController{}}
	property var dataController: null;
	
	signal selectedIndexChanged(int index);
	signal tableViewParamsAccepted();
	
	signal elementsChanged();
	signal headersChanged();
	signal tableDecoratorChanged()

	signal filterChanged(string filterId, var filterValue);
	
	signal selectionChanged(var ids, var indexes);
	signal checkedItemsChanged();
	signal rightButtonMouseClicked(int mouseX, int mouseY);
	signal doubleClicked(string id, int index);

	property bool activeFilter: false
	property int contentHeight: filterMenu_.height + 2 * filterMenu_.anchors.topMargin + (tableInternal.headerHeight + tableInternal.contentHeight) + paginationObj.height
	
	Component.onCompleted: {
		tableInternal.focus = true;
	}
	
	Connections {
		target: collectionViewBaseContainer.collectionFilter
		
		function onFilterChanged(){
			collectionViewBaseContainer.activeFilter = collectionViewBaseContainer.hasActiveFilter()
			collectionViewBaseContainer.doUpdateGui()
		}
	}
	
	Connections {
		target: collectionViewBaseContainer.documentCollectionFilter
		
		function onFilterChanged(){
			collectionViewBaseContainer.activeFilter = collectionViewBaseContainer.hasActiveFilter()
			collectionViewBaseContainer.doUpdateGui()
		}
	}
	
	function onModelChanged(){}
	
	function getSelectedIds(){
		return selectionManager.selectedIds
	}
	
	function hasActiveFilter(){
		if (collectionFilter){
			return !collectionFilter.isEmpty()
		}
		
		return false
	}
	
	function registerFieldFilterDelegate(filterId, filterDelegateComp){
		filterMenu_.registerFieldFilterDelegate(filterId, filterDelegateComp)
	}

	function setFilterDependency(filterId, dependsOnFilterId){
		filterMenu_.setFilterDependency(filterId, dependsOnFilterId)
	}

	FilterMenu {
		id: filterMenu_;
		anchors.top: parent.top;
		anchors.topMargin: Style.marginM;
		anchors.left: parent.left;
		anchors.right: parent.right;
		canResetFilters: collectionViewBaseContainer.canResetFilters
		complexFilter: collectionViewBaseContainer.collectionFilter;
		documentFilter: collectionViewBaseContainer.documentCollectionFilter;
		visible: collectionViewBaseContainer.filterMenuVisible
		onClose: {
			collectionViewBaseContainer.filterMenuVisible = false;
		}

		onFilterChanged: {
			collectionViewBaseContainer.activeFilter = collectionViewBaseContainer.hasActiveFilter()
			collectionViewBaseContainer.doUpdateGui();
		}
	}

	onHeadersChanged: {
		table.tableViewParams.clear();
		
		for (let i = 0; i < table.headers.getItemsCount(); i++){
			let headerId = table.getHeaderId(i);
			if (!table.tableViewParams.headerIsExists(headerId)){
				table.tableViewParams.setHeaderSize(headerId, -1);
				table.tableViewParams.setHeaderVisible(headerId, true);
				table.tableViewParams.setHeaderOrder(headerId, i);
			}
		}
	}
	
	Rectangle {
		id: backgroundTable;
		anchors.top: filterMenu_.visible ? filterMenu_.bottom: parent.top;
		anchors.left: parent.left;
		anchors.topMargin: filterMenu_.visible ? Style.marginM : 0;
		anchors.bottom: paginationObj.top;
		width: tableInternal.minWidth * tableInternal.columnCount < parent.width ? tableInternal.minWidth * tableInternal.columnCount : parent.width;
		color: Style.baseColor;
		radius: Style.size_mainCornerRadius !== undefined ? Style.size_mainCornerRadius: 0;
		
		TreeItemModel{
			id: tableDecoratorModel;
			
			property bool compl: false;
			property bool complCompl: tableDecoratorModel.compl && loaderTableDecorator.compl && tableInternal.headersCompl;
			
			Component.onCompleted: {
				tableDecoratorModel.compl = true;
			}
			
			onComplComplChanged: {
				if(tableDecoratorModel.complCompl){
					
					var tableDecorator = loaderTableDecorator.item;
					var count = tableInternal.headers.getItemsCount();
					
					if(tableDecorator.getItemsCount()){
						
						var headers = tableDecoratorModel.addTreeModel("Headers");
						var cells = tableDecoratorModel.addTreeModel("Cells");
						var cellWidth = tableDecoratorModel.addTreeModel("CellWidth");
						
						var general;
						if(tableDecorator.isValidData("General")){
							general = tableDecorator.getTreeItemModel("General")
							let keys = general.getKeys();
							for(let i = 0; i < keys.length; i++){
								if(tableInternal[keys[i]] !== undefined){
									tableInternal[keys[i]] = general.getData(keys[i]);
								}
							}
						}
						
						for(let i = 0; i < count; i++){
							headers.insertNewItem();
							cells.insertNewItem();
							cellWidth.insertNewItem();
							headers.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Headers"));
							cells.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Cells"));
							//cellWidth.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("CellWidth"));
							var widthVal = tableDecorator.getTreeItemModel("CellWidth").isValidData("Width",i) ?
										tableDecorator.getTreeItemModel("CellWidth").getData("Width",i) : -1;
							cellWidth.setData("Width", widthVal,i);
							
						}
						
						tableInternal.tableDecorator = tableDecoratorModel;
					}
				}
			}
		}
		
		Loader{
			id: loaderTableDecorator;
			sourceComponent: Style.collectionTableDecorator;
			
			property bool compl: false;
			
			onLoaded: {
				if(loaderTableDecorator.item){
					loaderTableDecorator.compl = true;
				}
			}
		}
		
		Table {
			id: tableInternal;
			anchors.left: parent.left;
			anchors.right: tableRightPanel.left;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			hasFilter: collectionViewBaseContainer.hasFilter;
			hasSort: collectionViewBaseContainer.hasSort;
			scrollbarVisible: false;
			canSetBorderParams: true;
			canSetBorderParams_deleg: true;
			canMoveColumns: true;
			scrollbarRightMargin: collectionViewBaseContainer.hasFilter ? -20 : 0;
			verticalBorderHeight: headerHeight / 2;
			verticalBorderSize: 1;
			visibleLeftBorderFirst: false;
			borderColorVertical: Style.borderColor;
			
			property real minWidth: 1000000;
			property bool headersCompl: false;
			property bool selectionBlock: false
			
			onSelectionChanged: {
				if (selectionBlock){
					return
				}

				let count = !elements ? 0 : elements.getItemsCount()
				if (count > 0){
					let selectedIds = []
					for (let i = 0; i < selection.length; i++){
						let elementId = elements.getData("id", selection[i])
						selectedIds.push(elementId)
					}
					collectionViewBaseContainer.selectionManager.selectMultiple(selectedIds)
				}
				else{
					collectionViewBaseContainer.selectionManager.clear()
				}
				
				collectionViewBaseContainer.selectionChanged(collectionViewBaseContainer.selectionManager.selectedIds, selectionManager.selectedIndexes);
			}
			
			onCheckedItemsChanged: {
				collectionViewBaseContainer.checkedItemsChanged();
			}
			
			onElementsChanged: {
				selectionBlock = true
				resetSelection()

				let elementsModel = collectionViewBaseContainer.dataController.elementsModel
				let count = elementsModel.getItemsCount()
				let selectedIndexes = []
				for (let i = 0; i < count; i++){
					let elementId = elementsModel.getData("id", i)
					if (collectionViewBaseContainer.selectionManager.isSelected(elementId)){
						selectedIndexes.push(i)
					}
				}

				selectionManager.selectMultiple(selectedIndexes)
				
				selectionBlock = false

				collectionViewBaseContainer.elementsChanged();
			}
			
			onHeadersChanged: {
				collectionViewBaseContainer.headersChanged();
				if(tableInternal.headers.getItemsCount()){
					tableInternal.headersCompl = true;
				}
			}

			onTableDecoratorChanged: {
				collectionViewBaseContainer.tableDecoratorChanged();
			}
			
			onHeaderRightMouseClicked: {
				if(collectionViewBaseContainer.headerRightClickEnabled){
					ModalDialogManager.openDialog(tableHeaderParamComp, {});
				}
			}
			
			onRightButtonMouseClicked: {
				collectionViewBaseContainer.rightButtonMouseClicked(mouseX, mouseY);
			}
			
			onDoubleClicked: {
				collectionViewBaseContainer.doubleClicked(id, index);
			}
			
			onSortingChanged: {
				if (collectionViewBaseContainer.collectionFilter){
					collectionViewBaseContainer.collectionFilter.setSortingInfo(headerId, sortOrder);
					collectionViewBaseContainer.collectionFilter.filterChanged();
				}
			}
		}
		
		Item {
			id: tableRightPanel;
			anchors.top: parent.top;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;
			width: collectionViewBaseContainer.hasFilter ? 35 : 0;
			visible: tableInternal.width > 0;
			
			Item {
				id: filterItem;
				anchors.top: parent.top;
				width: parent.width;
				height: tableInternal.headerHeight;

				ToolButton {
					id: iconFilter;
					objectName: "FilterVisible"
					anchors.centerIn: parent;
					width: Style.buttonWidthL;
					height: width;
					visible: collectionViewBaseContainer.hasFilter;
					icon.source: collectionViewBaseContainer.activeFilter ? "qrc:/" + Style.getIconPath("Icons/FilterActive", Icon.State.On, Icon.Mode.Selected)
										 : "qrc:/" + Style.getIconPath("Icons/FilterEdit", Icon.State.On, Icon.Mode.Normal)
					onClicked: {
						collectionViewBaseContainer.filterMenuVisible = !collectionViewBaseContainer.filterMenuVisible;
					}
					decorator: Component {
						ToolButtonDecorator {
							icon.width: 26
						}
					}
				}
			}
		}
	}
	
	Component {
		id: tableHeaderParamComp;
		
		TableHeaderParamComp{
			tableItem: tableInternal;
			onFinished: {
				if (buttonId === Enums.apply || buttonId === Enums.ok){
					if (contentItem){
						collectionViewBaseContainer.tableViewParamsAccepted();
					}
				}
			}
		}
	}
	
	Loading {
		id: loading_;
		anchors.fill: backgroundTable;
		color: Style.baseColor;
		visible: false;
	}
	
	Rectangle {
		id: paginationObj;
		anchors.bottom: parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: parent.width;
		height: !visible ? 0 : Style.controlHeightM;
		color: pagination_.color;
		visible: collectionViewBaseContainer.hasPagination
		
		Pagination {
			id: pagination_;
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.verticalCenter: parent.verticalCenter;
			pagesSize: 1;

			onCurrentIndexChanged: {
				tableInternal.resetSelection();
				collectionViewBaseContainer.doUpdateGui();
			}
			
			onCountElementsChanged: {
				tableInternal.selectedIndex = -1;
				collectionViewBaseContainer.doUpdateGui();
			}
		}
	}
}
