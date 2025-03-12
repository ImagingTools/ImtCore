import QtQuick 2.12
import Acf 1.0
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
	property alias isMultiCheckable: tableInternal.isMultiCheckable;
	
	property alias filterMenu: filterMenu_;
	property alias loading: loading_;
	property alias filterMenuVisible: filterMenu_.visible;
	property alias pagination: pagination_;
	property alias elementsCount: tableInternal.elementsCount;
	
	property JsonCollectionFilter collectionFilter: JsonCollectionFilter {}
	
	signal selectedIndexChanged(int index);
	signal tableViewParamsAccepted();
	
	signal elementsChanged();
	signal headersChanged();
	
	signal filterChanged(string filterId, var filterValue);
	
	signal selectionChanged(var selection);
	signal checkedItemsChanged();
	signal rightButtonMouseClicked(int mouseX, int mouseY);
	signal doubleClicked(string id, int index);
	
	Component.onCompleted: {
		tableInternal.focus = true;
	}
	
	Connections {
		target: collectionViewBaseContainer.collectionFilter;
		
		function onFilterChanged(){
			// console.log("onFilterChanged")
			
			// collectionViewBaseContainer.doUpdateGui();

			tableInternal.currentHeaderId = collectionViewBaseContainer.collectionFilter.getSortingInfoId();
			tableInternal.currentSortOrder = collectionViewBaseContainer.collectionFilter.getSortingOrder();
			
			// let sortingInfo = collectionViewBaseContainer.collectionFilter.getSortingInfo();
			// if (sortingInfo){
			// 	tableInternal.currentHeaderId = sortingInfo.m_fieldId;
			// 	tableInternal.currentSortOrder = sortingInfo.m_sortingOrder;
			// }
		}
	}
	
	function onModelChanged(){}
	
	FilterMenu {
		id: filterMenu_;
		
		anchors.top: parent.top;
		anchors.topMargin: Style.sizeMainMargin;
		anchors.left: parent.left;
		anchors.leftMargin: Style.sizeMainMargin;
		anchors.right: parent.right;
		anchors.rightMargin: Style.sizeMainMargin;
		
		// complexFilter: collectionViewBaseContainer.collectionFilter;
		
		onClose: {
			filterMenu_.visible = false;
		}
		
		onFilterChanged: {
			if (filterId == "TextFilter"){
				collectionViewBaseContainer.collectionFilter.setTextFilter(filterValue);
				collectionViewBaseContainer.doUpdateGui();
			}
			else if (filterId == "TimeFilter"){
				collectionViewBaseContainer.collectionFilter.m_timeFilter.
				collectionViewBaseContainer.collectionFilter.setTimeFilter(filterValue);
				collectionViewBaseContainer.doUpdateGui();
			}
			else{
				collectionViewBaseContainer.filterChanged(filterId, filterValue);
			}
		}
	}
	
	onHeadersChanged: {
		table.tableViewParams.clear();
		
		for (let i = 0; i < table.headers.getItemsCount(); i++){
			let headerId = table.headers.getData("Id", i);
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
		anchors.topMargin: filterMenu_.visible ? Style.sizeMainMargin : 0;
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
							var widthVal = tableDecorator.getTreeItemModel("CellWidth").isValidData("Width",i) ? tableDecorator.getTreeItemModel("CellWidth").getData("Width",i) : -1;
							cellWidth.setData("Width", widthVal,i);
							
						}
						
						tableInternal.tableDecorator = tableDecoratorModel;
					}
				}
			}
		}
		
		Loader{
			id: loaderTableDecorator;
			
			property bool compl: false;
			sourceComponent: Style.collectionTableDecorator;
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
			property real minWidth: 1000000;
			property bool headersCompl: false;
			
			canMoveColumns: true;
			
			scrollbarRightMargin: -20;
			verticalBorderHeight: headerHeight / 2;
			verticalBorderSize: 1;
			visibleLeftBorderFirst: false;
			borderColorVertical: Style.borderColor;
			
			onSelectionChanged: {
				collectionViewBaseContainer.selectionChanged(selection);
			}
			
			onCheckedItemsChanged: {
				collectionViewBaseContainer.checkedItemsChanged();
			}
			
			onElementsChanged: {
				collectionViewBaseContainer.elementsChanged();
			}
			
			onHeadersChanged: {
				collectionViewBaseContainer.headersChanged();
				if(tableInternal.headers.getItemsCount()){
					tableInternal.headersCompl = true;
				}
			}
			
			onHeaderRightMouseClicked: {
				ModalDialogManager.openDialog(tableHeaderParamComp, {});
			}
			
			onRightButtonMouseClicked: {
				collectionViewBaseContainer.rightButtonMouseClicked(mouseX, mouseY);
			}
			
			onDoubleClicked: {
				collectionViewBaseContainer.doubleClicked(id, index);
			}
			
			onCurrentHeaderIdChanged: {
				collectionViewBaseContainer.collectionFilter.setSortingInfoId(currentHeaderId);
				// if (collectionViewBaseContainer.collectionFilter){
				// 	collectionViewBaseContainer.collectionFilter.setSortingInfo(currentHeaderId, currentSortOrder);
				// 	collectionViewBaseContainer.collectionFilter.filterChanged();
				// }
			}
			
			onCurrentSortOrderChanged: {
				collectionViewBaseContainer.collectionFilter.setSortingOrder(currentSortOrder);
				// if (collectionViewBaseContainer.collectionFilter){
				// 	collectionViewBaseContainer.collectionFilter.setSortingInfo(currentHeaderId, currentSortOrder);
				// 	collectionViewBaseContainer.collectionFilter.filterChanged();
				// }
			}
			
			onHeaderClicked: {
				collectionViewBaseContainer.doUpdateGui();
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
					
					anchors.centerIn: parent;
					
					width: Style.buttonWidthMedium;
					height: width;
					
					visible: collectionViewBaseContainer.hasFilter;
					
					iconSource: "../../../" + Style.getIconPath("Icons/Filter", Icon.State.On, Icon.Mode.Normal);
					
					onClicked: {
						filterMenu_.visible = !filterMenu_.visible;
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
		height: collectionViewBaseContainer.hasPagination ? 30: 0;
		
		color: Style.baseColor;
		
		Pagination {
			id: pagination_;
			
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.verticalCenter: parent.verticalCenter;
			
			pagesSize: 1;
			
			visible: collectionViewBaseContainer.hasPagination /*&& pagesSize > 1*/;
			
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
