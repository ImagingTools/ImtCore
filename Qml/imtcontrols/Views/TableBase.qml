import QtQuick
import Acf 1.0
import imtcontrols 1.0

Rectangle {
	id: tableContainer;

	color: "transparent";

	property int selectedIndex: -1;
	property int itemHeight: 35;
	property int headerHeight: 35;
	property int headerMinHeight: 35;
	property int headerGap: 0;

	property bool hasFilter: false;
	property bool hasSort: false;
	property bool checkable: false;
	property bool selectable: true;
	property bool isMultiCheckable: true;
	property bool canSelectAll: true;
	property bool scrollbarVisible: true;
	property bool showHeaders: true;

	property bool enableAlternating: (Style.enableAlternating !== undefined && Style.enableAlternating !== null) ? Style.enableAlternating : false;
	property color alternatingColor: Style.alternatingColor ? Style.alternatingColor : '#000';
	property color alternatingCellColor: 'transparent';
	property real alternatingOpacity: Style.alternatingOpacity ? Style.alternatingOpacity : 0.05;
	property bool hoverEnabled: (Style.enableHoverEffect !== undefined && Style.enableHoverEffect !== null) ? Style.enableHoverEffect : true;

	property alias separatorVisible: bottomLine.visible;
	property string sortIndicatorIcon: "../../../" +  Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

	property TableViewParams tableViewParams: TableViewParams {}

	property TreeItemModel headers
	property TreeItemModel defaultHeadersModel: null;

	property TreeItemModel tableDecorator

	property TreeItemModel headerDecorator
	property TreeItemModel cellDecorator
	property TreeItemModel widthDecorator: TreeItemModel{}
	property TreeItemModel widthDecoratorDynamic: TreeItemModel{}

	property alias scrollbarItem: scrollbar;
	property alias scrollbarItemHoriz: scrollHoriz;

	property alias rowDelegate: elementsListObj.delegate;
	property alias elements: elementsListObj.model;
	property alias elementsSpacing: elementsListObj.spacing;

	property alias elementsList: elementsListObj;
	property alias cacheBuffer: elementsListObj.cacheBuffer;
	property alias contentHeight: elementsListObj.contentHeight;
	property real contentWidth: elementsListObj.contentWidth;
	property alias originX: elementsListObj.originX;
	property alias originY: elementsListObj.originY;
	property alias contentX: elementsListObj.contentX;
	property alias contentY: elementsListObj.contentY;
	property alias elementsListWidth: elementsListObj.width;
	property alias elementsCount: elementsListObj.count;
	property alias reuseItems: elementsListObj.reuseItems

	property alias headerDelegate: headersList.delegate;
	property real headerElementWidth:  (headersList.width)/headersList.count;
	property alias headerElementHeight: headersList.height;

	property alias backgroundElementsColor: elementsBg.color;
	property alias backgroundHeadersColor: headersPanel.color;

	property bool emptyDecorCell: true;

	/*property int */radius: 7;

	property int columnCount: 0;

	//
	property string borderColorHorizontal: "transparent";
	property string borderColorVertical: "black";
	property int horizontalBorderSize: 0;
	property int verticalBorderSize: 0;
	property int verticalBorderHeight: -1;

	property bool visibleLeftBorderFirst: true;
	property bool visibleRightBorderLast: false;
	property bool visibleTopBorderFirst: false;
	property bool visibleBottomBorderLast: true;

	property bool canSetBorderParams: false;
	property int wrapMode: Text.NoWrap;
	property int elideMode: Text.ElideRight;
	property bool isRightBorder: false;

	property bool isAllItemChecked: false;

	property bool readOnly: false;
	property bool editableHeaderParams: false;
	property bool canFitHeight : false;
	property bool canMoveColumns : false;
	property bool canSwapColumns : true;
	property bool isFrameScrolling : false;
	property int minCellWidth : 30;

	property string currentHeaderId;
	property string currentSortOrder: "ASC";
	property var nonSortableColumns: [];

	property bool isMultiSelect: true;

	property TableSelection tableSelection: TableSelection {
		onSelectionChanged: {
			tableContainer.selectionChanged(tableContainer.tableSelection.selectedIndexes);
		}
	}

	property TableProperties properties: TableProperties {
		onCheckedItemsChanged: {
			tableContainer.isAllItemChecked = tableContainer.isAllChecked();
			tableContainer.checkedItemsChanged();
		}
	}

	//properties for delegate:
	property string borderColorHorizontal_deleg: "transparent";
	property string borderColorVertical_deleg: "transparent";
	property int horizontalBorderSize_deleg: 0;
	property int verticalBorderSize_deleg: 0;

	property bool visibleLeftBorderFirst_deleg: true;
	property bool visibleRightBorderLast_deleg: false;
	property bool visibleTopBorderFirst_deleg: false;
	property bool visibleBottomBorderLast_deleg: true;

	property bool canSetBorderParams_deleg: canSetBorderParams;
	property int wrapMode_deleg: Text.NoWrap;
	property int elideMode_deleg: Text.ElideRight;
	property bool isRightBorder_deleg: false;

	property int textMarginHor_deleg: 8;
	property int textMarginVer_deleg: 8;
	//properties for delegate

	property string maxLengthText: '';

	property int textMarginHor: Style.sizeMainMargin;
	property int textMarginVer: Style.sizeMainMargin;

	property bool emptyDecor: true;
	property bool emptyDecorHeader: true;

	property var columnContentComps: ({});
	property var registeredDrawCellFunctions: ({});

	property bool compl: false;

	property bool isFlickable: false;
	property int defaultColumnWidth: 200;

	property  Component cellDelegate

	signal checkedItemsChanged();
	signal selectionChanged(var selection);

	signal clicked(int index);
	signal rightButtonMouseClicked(int mouseX, int mouseY);
	signal doubleClicked(string id, int index);
	signal setActiveFocusFromTable();
	signal headerClicked(string headerId);
	signal headerRightMouseClicked(string headerId);
	signal textFilterChanged(string id, int index, string text);
	signal filterClicked();
	signal saveWidth();
	signal sortingChanged(string headerId, string sortOrder);

	signal widthRecalc();
	signal heightRecalc();

	signal tableViewParamsAccepted();

	signal modelRefresh(int rowIndex, string modelRole);

	Component.onCompleted: {
		tableContainer.headerMinHeight = tableContainer.headerHeight;
		tableContainer.setWidth();

		tableContainer.compl = true;
	}

	onFocusChanged: {
		if (tableContainer.focus){
			elementsListObj.forceActiveFocus();
		}
	}

	onTableDecoratorChanged: {
		setDecorators();
	}

	onWidthChanged: {
		if (!visible){
			return;
		}

		tableContainer.setWidth();
		if(tableContainer.wrapMode !== Text.NoWrap){
			pauseHeight.stop();
			pauseHeight.start();
		}
	}

	onHeadersChanged: {
		let keys = Object.keys(columnContentComps);
		for (let i = 0; i < tableContainer.headers.getItemsCount(); i++){
			let headerId = tableContainer.headers.getData("id", i)
			if (!keys.includes(headerId)){
				tableContainer.columnContentComps[headerId] = null;
			}
		}

		if (defaultHeadersModel == null && headers){
			if (headers.getItemsCount() > 0){
				defaultHeadersModel = headers.copyMe();
			}
		}

		tableContainer.setWidth();
	}

	onElementsChanged: {
		properties._checkedItems = []

		tableContainer.setWidth();
	}
	
	onVisibleChanged: {
		if (visible){
			tableContainer.setWidth();
		}
	}

	onSelectionChanged: {
		if (selection.length > 0){
			let maxIndex = selection.sort()[selection.length - 1]

			elementsListObj.positionViewAtIndex(maxIndex, ListView.Contain)
		}
	}

	onContentWidthChanged: {
		if(tableContainer.isFlickable){
			elementsListObj.contentWidth = contentWidth;
		}
	}

	function setSortingInfo(headerId, sortOrder){
		currentHeaderId = headerId;
		currentSortOrder = sortOrder;
		
		sortingChanged(currentHeaderId, currentSortOrder);
	}

	function setDecorators(){
		if (!tableContainer.tableDecorator){
			return;
		}

		if (!tableContainer.headers){
			return;
		}

		tableContainer.headerDecorator = tableContainer.tableDecorator.getTreeItemModel("Headers");
		tableContainer.cellDecorator = tableContainer.tableDecorator.getTreeItemModel("Cells");
		tableContainer.widthDecorator = tableContainer.tableDecorator.getTreeItemModel("CellWidth");

		if (tableContainer.cellDecorator){
			tableContainer.emptyDecorCell = !tableContainer.cellDecorator.getItemsCount()
		}

		tableContainer.emptyDecor = !tableContainer.tableDecorator.getItemsCount();

		if (tableContainer.headerDecorator){
			tableContainer.emptyDecorHeader = !tableContainer.headerDecorator.getItemsCount();
		}

		tableContainer.setBorderParams();

		tableContainer.setWidth();

		if(tableContainer.wrapMode !== Text.NoWrap){
			for(var i = 0; i < tableContainer.headers.getItemsCount(); i++){
				heightModel.append({"cellHeight": 0});
			}
			tableContainer.heightRecalc();
			pauseHeight.stop();
			pauseHeight.start();
		}
	}

	function getSelectedIndexes(){
		return tableContainer.tableSelection.selectedIndexes;
	}

	function setSelectedIndexes(indexes){
		return tableContainer.tableSelection.selectedIndexes;
	}

	function select(index){
		tableContainer.tableSelection.singleSelect(index);
	}

	function resetSelection(){
		tableContainer.tableSelection.resetSelection();
	}

	//

	function checkItem(index){
		if (!tableContainer.checkable){
			return;
		}

		if (tableContainer.isMultiCheckable){
			tableContainer.properties.addCheckedItem(index);
		}
		else{
			tableContainer.properties.addSingleCheckedItem(index);
		}
	}

	function uncheckItem(index){
		if (!tableContainer.checkable){
			return;
		}

		tableContainer.properties.removeCheckedItem(index);
	}

	function itemIsChecked(index){
		return tableContainer.properties.itemIsChecked(index);
	}

	function uncheckAll(){
		if (!tableContainer.checkable){
			return;
		}

		tableContainer.properties.clearCheckedItems();
	}

	function checkAll(){
		if (!tableContainer.checkable || !tableContainer.isMultiCheckable){
			return;
		}

		let indexes = []
		for (let i = 0; i < elementsList.count; i++){
			indexes.push(i);
		}

		tableContainer.properties.addCheckedItems(indexes);
	}

	function isAllChecked(){
		if (!tableContainer.checkable){
			return false;
		}

		for (let i = 0; i < elementsList.count; i++){
			let isChecked = tableContainer.itemIsChecked(i);
			if (!isChecked){
				return false;
			}
		}

		return true;
	}

	function getCheckedItems(){
		return tableContainer.properties._checkedItems;
	}

	function setIsEnabledItem(index, isEnabled){
		tableContainer.properties.setIsEnabledItem(index, isEnabled);
	}

	function allEnabled(){
		tableContainer.properties.clearDisabledItems();
	}

	function setVisibleItem(index, isVisible){
		tableContainer.properties.setIsVisibleItem(index, isVisible);
	}

	function allVisible(){
		tableContainer.properties.clearInvisibleItems();
	}

	function getHeaderIndex(headerId){
		if (!tableContainer.headers){
			return -1;
		}

		for (let i = 0; i < tableContainer.headers.getItemsCount(); i++){
			if (tableContainer.headers.getData("id", i) === headerId){
				return i;
			}
		}

		return -1;
	}

	function registerFunctionDrawCellDelegate(columnId, func){
		registeredDrawCellFunctions[columnId] = func;
	}

	function setColumnContentComponent(columnIndex, comp){
		if (tableContainer.headers && columnIndex >= 0 && columnIndex < tableContainer.headers.getItemsCount()){
			let headerId = tableContainer.headers.getData("id", columnIndex);

			setColumnContentById(headerId, comp);
		}
	}

	function setColumnContentById(headerId, comp){
		tableContainer.columnContentComps[headerId] = comp;
	}

	function setCellHeight(){
		var maxVal = 0;
		for(var i = 0; i < heightModel.count; i++){
			var currVal = heightModel.get(i).cellHeight;
			if(currVal > maxVal){
				maxVal = currVal;
			}
		}
		tableContainer.headerHeight = Math.max(maxVal, tableContainer.headerMinHeight);
	}

	function setWidth(){
		if (!tableContainer.widthDecorator || !tableContainer.headers || !tableContainer.widthDecoratorDynamic){
			return;
		}
		let totalWidth = 0;

		let headersCount = tableContainer.headers.getItemsCount();
		let tableWidth_ = elementsListObj.width;
		//        var tableWidth_ = Math.max(tableContainer.width, tableContainer.contentWidth)

		if(tableContainer.widthDecorator.getItemsCount() === 0 && headersCount !== 0){
			for(let ind = 0; ind < headersCount ; ind++){
				let index = tableContainer.widthDecorator.insertNewItem();
				tableContainer.widthDecorator.setData("Width",-1,index);
				tableContainer.widthDecorator.setData("WidthPercent",-1,index);
			}
		}

		tableContainer.widthDecoratorDynamic.clear();
		tableContainer.widthDecoratorDynamic.copy(tableContainer.widthDecorator);

		let count_ = 0; // Count column with auto width
		let visibleColumnCount = 0;
		let lengthMinus = 0; // Total column width without automatic width

		for(let i = 0; i < tableContainer.widthDecorator.getItemsCount(); i++){
			var width_ = tableContainer.widthDecorator.isValidData("Width",i) ? tableContainer.widthDecorator.getData("Width",i): -1;
			var widthPercent_ = tableContainer.widthDecorator.isValidData("WidthPercent",i) ? tableContainer.widthDecorator.getData("WidthPercent",i): -1;

			if (width_ !== 0 && widthPercent_ !== 0){
				visibleColumnCount++;
			}

			if((width_ === -1) && (widthPercent_ === -1)){
				count_++;
			}
			else{
				width_ = width_< 0 ? 0 : width_;
				let newWidth;
				if(widthPercent_ >= 0){
					newWidth = tableWidth_ * (widthPercent_ / 100);
				}
				else {
					newWidth = width_;
				}
				lengthMinus += newWidth;

			}
		}

		if((tableWidth_ /*- lengthMinus*/) < visibleColumnCount * tableContainer.minCellWidth || count_ == tableContainer.widthDecorator.getItemsCount()){
			tableContainer.widthDecoratorDynamic.clear();
			for(let ind = 0; ind < headersCount; ind++){
				let index = tableContainer.widthDecoratorDynamic.insertNewItem();

				let columnWidth = tableContainer.widthDecorator.getData("Width", ind);
				if (columnWidth === 0){
					tableContainer.widthDecoratorDynamic.setData("Width", 0, index);
				}
				else{
					let defWidth = tableContainer.isFlickable ? tableContainer.defaultColumnWidth : tableWidth_/visibleColumnCount;
					tableContainer.widthDecoratorDynamic.setData("Width", defWidth, index);
					totalWidth += defWidth;
				}
			}
		}
		else {
			for(let i = 0; i < tableContainer.widthDecorator.getItemsCount(); i++){
				let width_ = tableContainer.widthDecorator.isValidData("Width",i) ? tableContainer.widthDecorator.getData("Width",i): -1;
				let widthPercent_ = tableContainer.widthDecorator.isValidData("WidthPercent",i) ? tableContainer.widthDecorator.getData("WidthPercent",i): -1;

				if (widthPercent_ >= 0 || width_ < 0  && widthPercent_ >= 0){
					tableContainer.widthDecoratorDynamic.setData("Width", widthPercent_*tableWidth_/100,i);
				}
				else if(width_ < 0  && widthPercent_ < 0 ){
					let defWidth = tableContainer.isFlickable ? tableContainer.defaultColumnWidth : (tableWidth_ - lengthMinus)/count_;
					tableContainer.widthDecoratorDynamic.setData("Width",defWidth,i);
				}
				else if(width_ >= 0  && widthPercent_ < 0 || width_ >= 0  && widthPercent_ >= 0){
					tableContainer.widthDecoratorDynamic.setData("Width", width_,i);
				}
				totalWidth += tableContainer.widthDecoratorDynamic.getData("Width", i);

			}
		}
		if(tableContainer.isFlickable){
			tableContainer.contentWidth = totalWidth;
		}
		tableContainer.widthRecalc();
	}

	function setBorderParams(){
		if(tableContainer.emptyDecorHeader){
			return;
		}
		if(!tableContainer.canSetBorderParams){
			return;
		}

		if(tableContainer.headerDecorator.isValidData("BorderColorHorizontal")){
			tableContainer.borderColorHorizontal = tableContainer.headerDecorator.getData("BorderColorHorizontal");
		}
		if(tableContainer.headerDecorator.isValidData("BorderColorVertical")){
			tableContainer.borderColorVertical = tableContainer.headerDecorator.getData("BorderColorVertical");
		}
		if(tableContainer.headerDecorator.isValidData("HorizontalBorderSize")){
			tableContainer.horizontalBorderSize = tableContainer.headerDecorator.getData("HorizontalBorderSize");
		}
		if(tableContainer.headerDecorator.isValidData("VerticalBorderSize")){
			tableContainer.verticalBorderSize = tableContainer.headerDecorator.getData("VerticalBorderSize");
		}
		if(tableContainer.headerDecorator.isValidData("VisibleLeftBorderFirst")){
			tableContainer.visibleLeftBorderFirst = tableContainer.headerDecorator.getData("VisibleLeftBorderFirst");
		}
		if(tableContainer.headerDecorator.isValidData("VisibleRightBorderLast")){
			tableContainer.visibleRightBorderLast = tableContainer.headerDecorator.getData("VisibleRightBorderLast");
		}
		if(tableContainer.headerDecorator.isValidData("VisibleTopBorderFirst")){
			tableContainer.visibleTopBorderFirst = tableContainer.headerDecorator.getData("VisibleTopBorderFirst");
		}
		if(tableContainer.headerDecorator.isValidData("VisibleBottomBorderLast")){
			tableContainer.visibleBottomBorderLast = tableContainer.headerDecorator.getData("VisibleBottomBorderLast");
		}
		if(tableContainer.headerDecorator.isValidData("WrapMode")){
			tableContainer.wrapMode = tableContainer.headerDecorator.getData("WrapMode");
		}
		if(tableContainer.headerDecorator.isValidData("IsRightBorder")){
			tableContainer.isRightBorder = tableContainer.headerDecorator.getData("IsRightBorder");
		}
		if(tableContainer.headerDecorator.isValidData("ElideMode")){
			tableContainer.elideMode = tableContainer.headerDecorator.getData("ElideMode");
		}
	}

	function getSelectedIds(){
		let retVal = []

		let indexes = tableContainer.getSelectedIndexes();
		for (let i = 0; i < indexes.length; i++){
			if (elementsListObj.model.containsKey("id", indexes[i])){
				let id = elementsListObj.model.getData("id", indexes[i]);
				retVal.push(id);
			}
		}

		return retVal;
	}

	function getSelectedNames(){
		let retVal = []

		let indexes = tableContainer.getSelectedIndexes();
		for (let i = 0; i < indexes.length; i++){
			if (elementsListObj.model.containsKey("name", indexes[i])){
				let name = elementsListObj.model.getData("name", indexes[i]);
				retVal.push(name);
			}
		}

		return retVal;
	}


	function getSelectedId(){
		if (tableContainer.selectedIndex > -1){
			let item = elementsListObj.itemAtIndex(tableContainer.selectedIndex);
			return item.getSelectedId();
		}

		return "";
	}

	function getSelectedName(){
		if (tableContainer.selectedIndex > -1){
			let item = elementsListObj.itemAtIndex(tableContainer.selectedIndex);
			return item.getSelectedName();
		}

		return "";
	}

	function getSelectedItemData(){
		if (tableContainer.selectedIndex > -1){
			let item = elementsListObj.itemAtIndex(tableContainer.selectedIndex);
			return item.getItemData();
		}

		return null;
	}


	function setContentHeight(content_height){
		elementsBg.contentHeight = content_height;
	}
	function getContentHeight(){
		return elementsBg.contentHeight;
	}


	PauseAnimation {
		id: pauseWidth;
		duration: 100;
		onFinished: {
			tableContainer.setWidth();
		}
	}

	PauseAnimation {
		id: pauseHeight;
		duration: 100;
		onFinished: {
			tableContainer.setCellHeight();
		}
	}

	ListModel{
		id: heightModel;
	}

	Rectangle {
		id: headersPanel;

		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: parent.top;

		height: visible ? tableContainer.headerHeight: 0;

		visible: headersList.count > 0 && tableContainer.showHeaders;

		color: Style.baseColor;

		clip: true;

		CheckBox {
			id: checkBox;

			z: 1000;

			anchors.verticalCenter: parent.verticalCenter;
			anchors.left: parent.left;
			anchors.leftMargin: Style.sizeMainMargin;

			visible: tableContainer.checkable && tableContainer.elementsList.count > 0 && tableContainer.canSelectAll;

			isActive: !tableContainer.readOnly;
			
			property bool isAllItemChecked: tableContainer.isAllItemChecked;
			
			onIsAllItemCheckedChanged: {
				checkState = isAllItemChecked ? Qt.Checked : Qt.Unchecked;
			}

			function nextCheckState(){
				if (tableContainer.readOnly){
					return;
				}
				if (checkBox.checkState === Qt.Unchecked){
					checkState = Qt.Checked;
					tableContainer.checkAll();
				}
				else{
					checkState = Qt.Unchecked;
					tableContainer.uncheckAll();
				}
			}
		}

		ListView {
			id: headersList;

			anchors.left: checkBox.visible ? checkBox.right : parent.left;
			anchors.right: parent.right;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;

			clip: true;
			reuseItems: true
			orientation: ListView.Horizontal;
			spacing: 0;
			model: tableContainer.headers;
			boundsBehavior: Flickable.StopAtBounds;
			contentX: tableContainer.elementsList.contentX;
			contentWidth: tableContainer.elementsList.contentWidth;
			interactive: false;
			cacheBuffer: Math.max(tableContainer.elementsList.contentWidth, tableContainer.width, 0);
			property bool compl: false;

			currentIndex: 0;

			Component.onCompleted: {
				headersList.compl = true;
			}
			onContentXChanged: {
				if(tableContainer.isFlickable){
					elementsListObj.contentX = headersList.contentX + elementsListObj.originX
				}
			}
			
			onModelChanged: {
				if (model){
					tableContainer.columnCount = model.getItemsCount();
				}
				else{
					tableContainer.columnCount = 0;
				}
			}

		}//Headers ListView

		Rectangle{
			id: bottomLine;

			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;

			height: 1;

			color: Style.borderColor;
			visible: tableContainer.emptyDecor;
		}
	}//headers

	Rectangle {
		id: elementsBg;

		anchors.fill: elementsListObj;
		color: tableContainer.color;//Style.baseColor;

		radius: tableContainer.radius;

		property real contentY: elementsListObj.contentY;
		property real originY: 0;
		property real contentWidth: elementsListObj.contentWidth;
		property real contentHeight: height;
	}

	property int scrollbarRightMargin: 0;

	CustomScrollbar {
		id: scrollbar;

		z: 100;

		anchors.right: parent.right;
		anchors.rightMargin: tableContainer.scrollbarRightMargin;
		anchors.bottom: elementsListObj.bottom;
		anchors.top: elementsListObj.top;

		secondSize: 10;
		targetItem: tableContainer.isFrameScrolling ? elementsBg : elementsListObj;

		onContentYSignal:{
			if(tableContainer.isFrameScrolling){
				elementsListObj.contentY = contentY;
			}
		}
	}

	CustomScrollbar{
		id: scrollHoriz;

		z: 101;

		anchors.right: elementsListObj.right;
		anchors.top: elementsListObj.bottom;
		anchors.topMargin: 1;

		secondSize: 10;
		allowableGapHor: 3;
		vertical: false;
		targetItem: elementsListObj;
	}

	ListView {
		id: elementsListObj;

		anchors.left: parent.left;
		anchors.right: scrollbar.visible ? scrollbar.left : parent.right;
		anchors.top: headersPanel.bottom;
		anchors.topMargin: tableContainer.headerGap;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: scrollHoriz.visible ? scrollHoriz.secondSize + scrollHoriz.anchors.topMargin : 0;

        boundsBehavior: Flickable.StopAtBounds;
		reuseItems: true

		// cacheBuffer: elementsListObj.count * tableContainer.itemHeight;

		clip: true;

		property bool scrollVisible: scrollbar.visible;
		onScrollVisibleChanged: {
			if(tableContainer.isFlickable){
				tableContainer.setWidth();
			}
		}

		onContentXChanged: {
			if(tableContainer.isFlickable){
				headersList.contentX = elementsListObj.contentX + headersList.originX
			}
		}

		onContentWidthChanged: {
			if(tableContainer.isFlickable){
				tableContainer.contentWidth = contentWidth;
			}
		}

		onContentYChanged: {
			if(tableContainer.isFrameScrolling){
				elementsBg.contentY = elementsListObj.contentY;
			}
		}
	}//Elements ListView

	function resetViewParams(){
		tableViewParams.clear();

		if (defaultHeadersModel){
			for (let i = 0; i < defaultHeadersModel.getItemsCount(); i++){
				let headerId = defaultHeadersModel.getData("id", i);
				tableViewParams.setHeaderSize(headerId, -1);
				tableViewParams.setHeaderVisible(headerId, true);
				tableViewParams.setHeaderOrder(headerId, i)
			}
		}

		updateWidthFromViewParams();
	}

	function updateWidthFromViewParams(){

		if(tableContainer.canSwapColumns){
			let swappedHeaderIndex = []
			for (let i = 0; i < tableContainer.tableViewParams.getItemsCount(); i++){
				let headerViewParamId = tableContainer.tableViewParams.getData("HeaderId", i);
				let index = getHeaderIndex(headerViewParamId);
				if (index >= 0 && i != index && !swappedHeaderIndex.includes(index)){
					tableContainer.widthDecorator.swapItems(i, index);
					tableContainer.headers.swapItems(i, index);
				}
			}
			headersList.model = 0;
			headersList.model = tableContainer.headers;
		}

		for (let i = 0; i < tableContainer.widthDecorator.getItemsCount(); i++){
			let headerId = tableContainer.headers.getData("id", i);
			for (let j = 0; j < tableContainer.tableViewParams.getItemsCount(); j++){
				let id = tableContainer.tableViewParams.getData("HeaderId", j);
				if (headerId === id){
					let visible = tableContainer.tableViewParams.getData("Visible", j);
					let size = tableContainer.tableViewParams.getData("Size", j);

					if (visible){
						tableContainer.widthDecorator.setData("WidthPercent", size, i)
						tableContainer.widthDecorator.setData("Width", size, i)
					}
					else{
						tableContainer.widthDecorator.setData("WidthPercent", 0, i)
						tableContainer.widthDecorator.setData("Width", 0, i)
					}

					break;

				}
			}
		}

		tableContainer.setWidth();

	}

	function fitToWidth(){
		if (!tableContainer.isFlickable ||!tableContainer.widthDecorator || !tableContainer.headers || !tableContainer.widthDecoratorDynamic){
			return;
		}

		let rowNumberColumnWidth = 0;
		let rowNumberColumnPercent = 0
		let restPercent = 0;
		let isVisibleRowNumberColumn = false;

		let headersCount = tableContainer.headers.getItemsCount();
		let tableWidth_ = elementsListObj.width;

		let visibleColumnCount = 0;

		for(let i = 0; i < tableContainer.widthDecoratorDynamic.getItemsCount(); i++){
			let width_ = tableContainer.widthDecoratorDynamic.isValidData("Width",i) ? tableContainer.widthDecoratorDynamic.getData("Width",i): -1;
			if (width_ !== 0){
				visibleColumnCount++;
				let headerId__ = tableContainer.headers.getData("id", i);
				if(headerId__ == "n"){
					rowNumberColumnWidth = width_;
				}
			}
		}

		for(let ind = 0; ind < headersCount; ind++){

			let columnWidth = tableContainer.widthDecoratorDynamic.getData("Width", ind);

			if(columnWidth <= 0){
				tableContainer.widthDecoratorDynamic.setData("Width", 0, ind);
				tableContainer.widthDecorator.setData("Width", 0, ind);
				tableContainer.widthDecorator.setData("WidthPercent", 0, ind);
			}
			else {
				isVisibleRowNumberColumn = rowNumberColumnWidth > 0;

				let isGeneralWidthForNumberColumn = !isVisibleRowNumberColumn ? true : (tableWidth_ - rowNumberColumnWidth)/(visibleColumnCount-1) < rowNumberColumnWidth

				let defWidth = isGeneralWidthForNumberColumn ? tableWidth_/visibleColumnCount :
															   (tableWidth_ - rowNumberColumnWidth)/(visibleColumnCount -1);

				let percent = (defWidth/tableWidth_) * 100;

				let headerId = tableContainer.headers.getData("id", ind);

				if(headerId == "n"){
					if(isVisibleRowNumberColumn){
						rowNumberColumnWidth  = isGeneralWidthForNumberColumn ? tableWidth_/visibleColumnCount : rowNumberColumnWidth;
						rowNumberColumnPercent = (rowNumberColumnWidth/tableWidth_) * 100;
						tableContainer.widthDecoratorDynamic.setData("Width", rowNumberColumnWidth, ind);
						tableContainer.widthDecorator.setData("Width", rowNumberColumnWidth, ind);
						tableContainer.widthDecorator.setData("WidthPercent", rowNumberColumnPercent, ind);

						tableViewParams.setHeaderSize(headerId, rowNumberColumnPercent)
					}
				}
				else {
					tableContainer.widthDecoratorDynamic.setData("Width", defWidth, ind);
					tableContainer.widthDecorator.setData("Width", defWidth, ind);
					tableContainer.widthDecorator.setData("WidthPercent", percent, ind);

					tableViewParams.setHeaderSize(headerId, percent)

					restPercent += percent;
				}

			}

		}//for

		tableContainer.saveWidth();

		tableContainer.contentWidth = tableWidth_;

		tableContainer.widthRecalc();

	}

	function fitToWidthOptional(){
		if(tableContainer.isFlickable){
			if(tableContainer.contentWidth < elementsListObj.width){
				fitToWidth();
			}
		}
	}
}


