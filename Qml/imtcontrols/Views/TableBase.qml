import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: tableContainer;

    color: "transparent";

    property int selectedIndex: -1;
    property int itemHeight: 35;
    property int headerHeight: 35;
    property int headerMinHeight: 35;

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
    property real alternatingOpacity: Style.alternatingOpacity ? Style.alternatingOpacity : 0.05;
    property bool hoverEnabled: (Style.enableHoverEffect !== undefined && Style.enableHoverEffect !== null) ? Style.enableHoverEffect : true;

    property alias separatorVisible: bottomLine.visible;
    property string sortIndicatorIcon: "../../../" +  Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

    property TreeItemModel headers

    property TreeItemModel tableDecorator

    property TreeItemModel headerDecorator
    property TreeItemModel cellDecorator
    property TreeItemModel widthDecorator: TreeItemModel{}
    property TreeItemModel widthDecoratorDynamic: TreeItemModel{}

    property alias scrollbarItem: scrollbar;

    property alias rowDelegate: elementsListObj.delegate;
    property alias elements: elementsListObj.model;

    property alias elementsList: elementsListObj;
    property alias cacheBuffer: elementsListObj.cacheBuffer;
    property alias contentHeight: elementsListObj.contentHeight;
    property alias contentWidth: elementsListObj.contentWidth;
    property alias originX: elementsListObj.originX;
    property alias originY: elementsListObj.originY;
    property alias contentX: elementsListObj.contentX;
    property alias contentY: elementsListObj.contentY;
    property alias elementsListWidth: elementsListObj.width;
    property alias elementsCount: elementsListObj.count;

    property alias headerDelegate: headersList.delegate;
    property real headerElementWidth:  (headersList.width)/headersList.count;
    property alias headerElementHeight: headersList.height;

    property alias backgroundElementsColor: elementsBg.color;
    property alias backgroundHeadersColor: headersPanel.color;

    property bool emptyDecorCell: true;

    /*property int */radius: 7;

    property alias columnCount: headersList.count;

    //
    property string borderColorHorizontal: "transparent";
    property string borderColorVertical: "transparent";
    property int horizontalBorderSize: 0;
    property int verticalBorderSize: 0;

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
    property bool canFitHeight : false;
    property bool canMoveColumns : false;
    property bool isFrameScrolling : false;
    property int minCellWidth : 30;

    property string currentHeaderId;
    property string currentSortOrder: "ASC";

    property alias isMultiSelect: tableContainer.tableSelection.isMultiSelect;

    property TableSelection tableSelection: TableSelection {
        onSelectionChanged: {
            console.log("TableSelection onSelectionChanged", tableContainer.tableSelection.selectedIndexes);
            tableContainer.selectionChanged(tableContainer.tableSelection.selectedIndexes);
        }
    }

    property TableProperties properties: TableProperties {
        onCheckedItemsChanged: {
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

    property int textMarginHor: 8;
    property int textMarginVer: 8;

    property bool emptyDecor: true;
    property bool emptyDecorHeader: true;

    property var columnContentComps: [];

    property bool compl: false;

    property  Component cellDelegate

    signal checkedItemsChanged();
    signal selectionChanged(var selection);

    signal clicked();
    signal rightButtonMouseClicked(int mouseX, int mouseY);
    signal doubleClicked(string id, int index);
    signal setActiveFocusFromTable();
    signal headerClicked(string headerId);
    signal textFilterChanged(string id, int index, string text);
    signal filterClicked();

    signal widthRecalc();
    signal heightRecalc();

    Component.onCompleted: {
        tableContainer.headerMinHeight = tableContainer.headerHeight;
        tableContainer.setWidth();

        tableContainer.tableSelection.tableItem = elementsListObj;
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
        tableContainer.setWidth();
        if(tableContainer.wrapMode !== Text.NoWrap){
            pauseHeight.stop();
            pauseHeight.start();
        }
    }

    onHeadersChanged: {
        tableContainer.columnContentComps = [];

        for (let i = 0; i < tableContainer.headers.GetItemsCount(); i++){
            tableContainer.columnContentComps.push(null);
        }

        setWidth();
    }

    onElementsChanged: {
        uncheckAll();
    }

    onSelectionChanged: {
        if (selection.length > 0){
            let maxIndex = selection.sort()[selection.length - 1]

            elementsListObj.positionViewAtIndex(maxIndex, ListView.Contain)
        }
    }

    function setDecorators(){
        console.log("onTableDecoratorChanged", tableContainer.tableDecorator, tableContainer.headers);
        if (!tableContainer.tableDecorator){
            return;
        }

        if (!tableContainer.headers){
            return;
        }

        tableContainer.headerDecorator = tableContainer.tableDecorator.GetTreeItemModel("Headers");
        tableContainer.cellDecorator = tableContainer.tableDecorator.GetTreeItemModel("Cells");
        tableContainer.widthDecorator = tableContainer.tableDecorator.GetTreeItemModel("CellWidth");

        if (tableContainer.cellDecorator){
            tableContainer.emptyDecorCell = !tableContainer.cellDecorator.GetItemsCount()
        }

        tableContainer.emptyDecor = !tableContainer.tableDecorator.GetItemsCount();

        if (tableContainer.headerDecorator){
            tableContainer.emptyDecorHeader = !tableContainer.headerDecorator.GetItemsCount();
        }

        tableContainer.setBorderParams();

        tableContainer.setWidth();

        if(tableContainer.wrapMode !== Text.NoWrap){
            for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
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
        let count = tableContainer.elements.GetItemsCount();
        for (let i = 0; i < count; i++){
            indexes.push(i);
        }

        tableContainer.properties.addCheckedItems(indexes);
    }

    function isAllChecked(){
        if (!tableContainer.checkable){
            return false;
        }

        let count = tableContainer.elements.GetItemsCount();
        for (let i = 0; i < count; i++){
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

        for (let i = 0; i < tableContainer.headers.GetItemsCount(); i++){
            if (tableContainer.headers.GetData("Id", i) === headerId){
                return i;
            }
        }

        return -1;
    }

    function setColumnContentComponent(columnIndex, comp){
        tableContainer.columnContentComps[columnIndex] = comp;
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

        var headersCount = tableContainer.headers.GetItemsCount();
        var tableWidth_ = Math.max(tableContainer.width, tableContainer.contentWidth);

        if(!tableContainer.widthDecorator.GetItemsCount() && tableContainer.headers.GetItemsCount()){
            for(var ind = 0; ind < headersCount ; ind++){
                var index = tableContainer.widthDecorator.InsertNewItem();
                tableContainer.widthDecorator.SetData("Width",-1,index);
                tableContainer.widthDecorator.SetData("WidthPercent",-1,index);
            }
        }

        tableContainer.widthDecoratorDynamic.Clear();
        tableContainer.widthDecoratorDynamic.Copy(tableContainer.widthDecorator);

        var count_ = 0;
        var lengthMinus = 0;

        for(var i = 0; i < tableContainer.widthDecorator.GetItemsCount(); i++){
            var width_ = tableContainer.widthDecorator.IsValidData("Width",i) ? tableContainer.widthDecorator.GetData("Width",i): -1;
            var widthPercent_ = tableContainer.widthDecorator.IsValidData("WidthPercent",i) ? tableContainer.widthDecorator.GetData("WidthPercent",i): -1;

            if((width_ == -1) && (widthPercent_ == -1)){
                count_++;
            }
            else{
                width_ = width_< 0 ? 0 : width_;
                widthPercent_ = widthPercent_ < 0 ? 0 : widthPercent_* tableWidth_/100;
                lengthMinus += Math.max(width_,widthPercent_);
            }
        }
        if((tableWidth_ - lengthMinus) < count_ * tableContainer.minCellWidth || count_ == tableContainer.widthDecorator.GetItemsCount() ){
            tableContainer.widthDecoratorDynamic.Clear();
            for(let ind = 0; ind < headersCount; ind++){
                let index = tableContainer.widthDecoratorDynamic.InsertNewItem();
                tableContainer.widthDecoratorDynamic.SetData("Width",tableWidth_/headersCount,index);
            }
            tableContainer.widthRecalc();
            return;
        }

        for(let i = 0; i < tableContainer.widthDecorator.GetItemsCount(); i++){
            let width_ = tableContainer.widthDecorator.IsValidData("Width",i) ? tableContainer.widthDecorator.GetData("Width",i): -1;
            let widthPercent_ = tableContainer.widthDecorator.IsValidData("WidthPercent",i) ? tableContainer.widthDecorator.GetData("WidthPercent",i): -1;

            if(width_ < 0  && widthPercent_ < 0 ){
                if(count_){
                    tableContainer.widthDecoratorDynamic.SetData("Width",(tableWidth_ - lengthMinus)/count_,i);
                }
            }
            else if(width_ < 0  && widthPercent_ >= 0){
                tableContainer.widthDecoratorDynamic.SetData("Width", widthPercent_*tableWidth_/100,i);
            }

            else if(width_ >= 0  && widthPercent_ < 0){
                tableContainer.widthDecoratorDynamic.SetData("Width", width_,i);
            }

            else if(width_ >= 0  && widthPercent_ >= 0){
                tableContainer.widthDecoratorDynamic.SetData("Width", width_,i);
            }

        }

        tableContainer.widthRecalc();
    }

    function saveWidth(){

    }

    function setBorderParams(){

        console.log("setBorderParams", tableContainer.emptyDecorHeader, tableContainer.canSetBorderParams)
        if(tableContainer.emptyDecorHeader){
            return;
        }
        if(!tableContainer.canSetBorderParams){
            return;
        }

        if(tableContainer.headerDecorator.IsValidData("BorderColorHorizontal")){
            tableContainer.borderColorHorizontal = tableContainer.headerDecorator.GetData("BorderColorHorizontal");
        }
        if(tableContainer.headerDecorator.IsValidData("BorderColorVertical")){
            tableContainer.borderColorVertical = tableContainer.headerDecorator.GetData("BorderColorVertical");
        }
        if(tableContainer.headerDecorator.IsValidData("HorizontalBorderSize")){
            tableContainer.horizontalBorderSize = tableContainer.headerDecorator.GetData("HorizontalBorderSize");
        }
        if(tableContainer.headerDecorator.IsValidData("VerticalBorderSize")){
            tableContainer.verticalBorderSize = tableContainer.headerDecorator.GetData("VerticalBorderSize");
        }
        if(tableContainer.headerDecorator.IsValidData("VisibleLeftBorderFirst")){
            tableContainer.visibleLeftBorderFirst = tableContainer.headerDecorator.GetData("VisibleLeftBorderFirst");
        }
        if(tableContainer.headerDecorator.IsValidData("VisibleRightBorderLast")){
            tableContainer.visibleRightBorderLast = tableContainer.headerDecorator.GetData("VisibleRightBorderLast");
        }
        if(tableContainer.headerDecorator.IsValidData("VisibleTopBorderFirst")){
            tableContainer.visibleTopBorderFirst = tableContainer.headerDecorator.GetData("VisibleTopBorderFirst");
        }
        if(tableContainer.headerDecorator.IsValidData("VisibleBottomBorderLast")){
            tableContainer.visibleBottomBorderLast = tableContainer.headerDecorator.GetData("VisibleBottomBorderLast");
        }
        if(tableContainer.headerDecorator.IsValidData("WrapMode")){
            tableContainer.wrapMode = tableContainer.headerDecorator.GetData("WrapMode");
        }
        if(tableContainer.headerDecorator.IsValidData("IsRightBorder")){
            tableContainer.isRightBorder = tableContainer.headerDecorator.GetData("IsRightBorder");
        }
        if(tableContainer.headerDecorator.IsValidData("ElideMode")){
            tableContainer.elideMode = tableContainer.headerDecorator.GetData("ElideMode");
        }

    }

    function getSelectedIds(){
        console.log("getSelectedIds");
        let retVal = []

        let indexes = tableContainer.getSelectedIndexes();
        for (let i = 0; i < indexes.length; i++){
            if (elementsListObj.model.ContainsKey("Id", indexes[i])){
                let id = elementsListObj.model.GetData("Id", indexes[i]);
                retVal.push(id);
            }
        }

        return retVal;
    }

    function getSelectedNames(){
        let retVal = []

        let indexes = tableContainer.getSelectedIndexes();
        for (let i = 0; i < indexes.length; i++){
            if (elementsListObj.model.ContainsKey("Name", indexes[i])){
                let name = elementsListObj.model.GetData("Name", indexes[i]);
                retVal.push(name);
            }
        }

        return retVal;
    }


    function getSelectedId(){
        console.log("getSelectedId", tableContainer.selectedIndex);
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
        // anchors.rightMargin: 5;
        anchors.top: parent.top;

        height: visible ? tableContainer.headerHeight: 0;

        visible: headersList.count > 0 && tableContainer.showHeaders;

        color: Style.baseColor;

        //radius: tableContainer.radius;

        clip: true;

        ListView {
            id: headersList;

            anchors.fill: parent;

            clip: true;
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

            onCurrentIndexChanged: {
                console.log("headersList onCurrentIndexChanged", headersList.currentIndex);
            }

            Component.onCompleted: {
                headersList.compl = true;
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
        color: Style.baseColor;

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

        anchors.right: elementsListObj.right;
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
        anchors.topMargin: 1 ;

        secondSize: 10;

        vertical: false;
        targetItem: elementsListObj;

        visible: elementsListObj.width > 0;
    }

    ListView {
        id: elementsListObj;

        anchors.left: parent.left;
        anchors.right: headersPanel.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: scrollHoriz.secondSize + scrollHoriz.anchors.topMargin;

        boundsBehavior: Flickable.StopAtBounds;

//        cacheBuffer: count * tableContainer.itemHeight;
        cacheBuffer: tableContainer.height + 5 * tableContainer.itemHeight;

        clip: true;

        onContentXChanged: {
            headersList.contentX = contentX + headersList.originX
        }

        Keys.onUpPressed: {
            tableContainer.tableSelection.up();
        }

        Keys.onDownPressed: {
            tableContainer.tableSelection.down();
        }

//        onActiveFocusChanged: {
//            if (elementsListObj.activeFocus){
//                tableContainer.tableSelection.subscribeEvents();
//            }
//            else{
//                tableContainer.tableSelection.unsubscribeEvents();
//            }
//        }

        onContentYChanged: {
            if(tableContainer.isFrameScrolling){
                elementsBg.contentY = elementsListObj.contentY;
            }
        }
    }//Elements ListView
}


