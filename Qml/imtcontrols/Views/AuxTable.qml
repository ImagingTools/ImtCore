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

    property TreeItemModel headers : TreeItemModel{};

    property TreeItemModel tableDecorator : TreeItemModel{};

    property TreeItemModel headerDecorator : TreeItemModel{};
    property TreeItemModel cellDecorator : TreeItemModel{};
    property TreeItemModel widthDecorator : TreeItemModel{};
    property TreeItemModel widthDecoratorDynamic : TreeItemModel{};

    property alias scrollbarItem: scrollbar;

    property alias delegate: elementsListObj.delegate;
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

    property  Component cellDelegate: TableCellDelegate {};

    signal checkedItemsChanged();
    signal selectionChanged(var selection);

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

        for (let i = 0; i < tableContainer.headers.getItemsCount(); i++){
            tableContainer.columnContentComps.push(null);
        }

        setWidth();
    }

    onElementsChanged: {
        uncheckAll();
    }

    onVisibleChanged: {
        if (visible){
            let indexes = tableContainer.tableSelection.selectedIndexes;
            if (indexes.length > 0){
                elementsListObj.positionViewAtIndex(indexes[0], ListView.Beginning)
            }
        }
    }

    function setDecorators(){
        console.log("onTableDecoratorChanged");
        if (!tableContainer.tableDecorator){
            return;
        }

        if (!tableContainer.headers){
            return;
        }

        tableContainer.headerDecorator = tableContainer.tableDecorator.getTreeItemModel("Headers");
        tableContainer.cellDecorator = tableContainer.tableDecorator.getTreeItemModel("Cells");
        tableContainer.widthDecorator = tableContainer.tableDecorator.getTreeItemModel("CellWidth");

        console.log("tableContainer.widthDecorator", tableContainer.widthDecorator);

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
        let count = tableContainer.elements.getItemsCount();
        for (let i = 0; i < count; i++){
            indexes.push(i);
        }

        tableContainer.properties.addCheckedItems(indexes);
    }

    function isAllChecked(){
        if (!tableContainer.checkable){
            return false;
        }

        let count = tableContainer.elements.getItemsCount();
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

        for (let i = 0; i < tableContainer.headers.getItemsCount(); i++){
            if (tableContainer.headers.getData("Id", i) === headerId){
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

        var headersCount = tableContainer.headers.getItemsCount();
        var tableWidth_ = Math.max(tableContainer.width, tableContainer.contentWidth);

        if(!tableContainer.widthDecorator.getItemsCount() && tableContainer.headers.getItemsCount()){
            for(var ind = 0; ind < headersCount ; ind++){
                var index = tableContainer.widthDecorator.insertNewItem();
                tableContainer.widthDecorator.setData("Width",-1,index);
                tableContainer.widthDecorator.setData("WidthPercent",-1,index);
            }
        }

        tableContainer.widthDecoratorDynamic.clear();
        tableContainer.widthDecoratorDynamic.copy(tableContainer.widthDecorator);

        var count_ = 0;
        var lengthMinus = 0;

        for(var i = 0; i < tableContainer.widthDecorator.getItemsCount(); i++){
            var width_ = tableContainer.widthDecorator.isValidData("Width",i) ? tableContainer.widthDecorator.getData("Width",i): -1;
            var widthPercent_ = tableContainer.widthDecorator.isValidData("WidthPercent",i) ? tableContainer.widthDecorator.getData("WidthPercent",i): -1;

            if((width_ == -1) && (widthPercent_ == -1)){
                count_++;
            }
            else{
                width_ = width_< 0 ? 0 : width_;
                widthPercent_ = widthPercent_ < 0 ? 0 : widthPercent_* tableWidth_/100;
                lengthMinus += Math.max(width_,widthPercent_);
            }
        }
        if((tableWidth_ - lengthMinus) < count_ * tableContainer.minCellWidth || count_ == tableContainer.widthDecorator.getItemsCount() ){
            tableContainer.widthDecoratorDynamic.clear();
            for(let ind = 0; ind < headersCount; ind++){
                let index = tableContainer.widthDecoratorDynamic.insertNewItem();
                tableContainer.widthDecoratorDynamic.setData("Width",tableWidth_/headersCount,index);
            }
            tableContainer.widthRecalc();
            return;
        }

        for(let i = 0; i < tableContainer.widthDecorator.getItemsCount(); i++){
            let width_ = tableContainer.widthDecorator.isValidData("Width",i) ? tableContainer.widthDecorator.getData("Width",i): -1;
            let widthPercent_ = tableContainer.widthDecorator.isValidData("WidthPercent",i) ? tableContainer.widthDecorator.getData("WidthPercent",i): -1;

            if(width_ < 0  && widthPercent_ < 0 ){
                if(count_){
                    tableContainer.widthDecoratorDynamic.setData("Width",(tableWidth_ - lengthMinus)/count_,i);
                }
            }
            else if(width_ < 0  && widthPercent_ >= 0){
                tableContainer.widthDecoratorDynamic.setData("Width", widthPercent_*tableWidth_/100,i);
            }

            else if(width_ >= 0  && widthPercent_ < 0){
                tableContainer.widthDecoratorDynamic.setData("Width", width_,i);
            }

            else if(width_ >= 0  && widthPercent_ >= 0){
                tableContainer.widthDecoratorDynamic.setData("Width", width_,i);
            }

        }

        tableContainer.widthRecalc();
    }

    function saveWidth(){

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
        console.log("getSelectedIds");
        let retVal = []

        let indexes = tableContainer.getSelectedIndexes();
        for (let i = 0; i < indexes.length; i++){
            if (elementsListObj.model.containsKey("Id", indexes[i])){
                let id = elementsListObj.model.getData("Id", indexes[i]);
                retVal.push(id);
            }
        }

        return retVal;
    }

    function getSelectedNames(){
        let retVal = []

        let indexes = tableContainer.getSelectedIndexes();
        for (let i = 0; i < indexes.length; i++){
            if (elementsListObj.model.containsKey("Name", indexes[i])){
                let name = elementsListObj.model.getData("Name", indexes[i]);
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

            delegate: Item{
                id: deleg;

                height: headersList.height;

                property bool compl: false;
                property bool complCompl: deleg.compl && headersList.compl;

                Component.onCompleted: {
                    deleg.compl = true;
                }
                Component.onDestruction: {
                    if(deleg.complCompl) tableContainer.widthRecalc.disconnect(deleg.setCellWidth);
                }
                onComplComplChanged: {
                    if(deleg.complCompl){
                        tableContainer.widthRecalc.connect(deleg.setCellWidth);
                        deleg.setCellWidth();
                    }

                }

                function setCellWidth(){
                    if(!deleg || !deleg.complCompl || !headersList){
                        return;
                    }

                    var defaultWidth = (headersList.width)/headersList.count;
                    var widthFromModel = tableContainer.widthDecoratorDynamic.isValidData("Width", model.index) ? tableContainer.widthDecoratorDynamic.getData("Width", model.index) : -1;


                    if(!tableContainer.widthDecoratorDynamic.getItemsCount()){
                        width = defaultWidth;
                    }
                    else if(widthFromModel >= 0){
                        width = widthFromModel;
                    }
                    else{
                        width = defaultWidth;
                    }

                }

                //borders

                Rectangle{
                    id: topBorder;
                    anchors.top: parent.top;
                    anchors.left: parent.left;
                    anchors.right: parent.right;
                    height: tableContainer.visibleTopBorderFirst  ? tableContainer.horizontalBorderSize : 0;
                    color: tableContainer.borderColorHorizontal;
                }

                Rectangle{
                    id: bottomBorder;
                    anchors.bottom: parent.bottom;
                    anchors.left: parent.left;
                    anchors.right: parent.right;
                    height: tableContainer.visibleBottomBorderLast ? tableContainer.horizontalBorderSize : 0;
                    color:  tableContainer.borderColorHorizontal;
                }

                Rectangle{
                    id: leftBorder;
                    anchors.left: parent.left;
                    anchors.top: parent.top;
                    anchors.bottom: parent.bottom;
                    width: tableContainer.isRightBorder ? tableContainer.verticalBorderSize * tableContainer.visibleLeftBorderFirst * (model.index == 0)
                                                        : tableContainer.visibleLeftBorderFirst ? tableContainer.verticalBorderSize : model.index > 0 ? tableContainer.verticalBorderSize : 0;
                    color:  tableContainer.borderColorVertical;
                }

                Rectangle{
                    id: rightBorder;
                    anchors.right: parent.right;
                    anchors.top: parent.top;
                    anchors.bottom: parent.bottom;
                    width: !tableContainer.isRightBorder ?
                               tableContainer.verticalBorderSize * tableContainer.visibleRightBorderLast  * (model.index == (headersList.count -1)) * (headersList.count > 0) :
                               tableContainer.visibleRightBorderLast ? tableContainer.verticalBorderSize  :
                                                                       tableContainer.verticalBorderSize * (model.index < (headersList.count -1));

                    color: tableContainer.borderColorVertical;
                }


                //borders

                Rectangle{
                    id: mainRec;
                    anchors.top: topBorder.bottom;
                    anchors.left: leftBorder.right;
                    anchors.right: rightBorder.left;
                    anchors.bottom: bottomBorder.top;
                    color: tableContainer.emptyDecorHeader ? "transparent" :
                                                             tableContainer.headerDecorator.isValidData("Color", model.index) ?
                                                                 tableContainer.headerDecorator.getData("Color", model.index) :
                                                                 "transparent";

                    opacity:  tableContainer.emptyDecorHeader ? 1 :
                                                                tableContainer.headerDecorator.isValidData("Opacity", model.index) ?
                                                                    tableContainer.headerDecorator.getData("Opacity", model.index) :
                                                                    1;

                    radius: tableContainer.emptyDecorHeader ? 0 :
                                                              tableContainer.headerDecorator.isValidData("CellRadius", model.index) ?
                                                                  tableContainer.headerDecorator.getData("CellRadius", model.index) :0;




                    //cornerPatches
                    Rectangle{
                        id: leftTopCornerPatch;
                        anchors.left: parent.left;
                        anchors.top: parent.top;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.isValidData("LeftTopRound", model.index) ?
                                                                       !tableContainer.headerDecorator.getData("LeftTopRound", model.index) :true;
                    }

                    Rectangle{
                        id: rightTopCornerPatch;
                        anchors.right: parent.right;
                        anchors.top: parent.top;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.isValidData("RightTopRound", model.index) ?
                                                                       !tableContainer.headerDecorator.getData("RightTopRound", model.index) :true;


                    }

                    Rectangle{
                        id: leftBottomCornerPatch;
                        anchors.left: parent.left;
                        anchors.bottom: parent.bottom;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.isValidData("LeftBottomRound", model.index) ?
                                                                       !tableContainer.headerDecorator.getData("LeftBottomRound", model.index) :true;
                    }

                    Rectangle{
                        id: rightBottomCornerPatch;
                        anchors.right:  parent.right;
                        anchors.bottom: parent.bottom;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.isValidData("RightBottomRound", model.index) ?
                                                                       !tableContainer.headerDecorator.getData("RightBottomRound", model.index) :true;
                    }
                    //cornerPatches



                }//mainRec

                CheckBox {
                    id: checkBox;

                    z: 1000;

                    anchors.verticalCenter: mainRec.verticalCenter;
                    anchors.left: mainRec.left;
                    anchors.leftMargin: 8;

                    checkState: tableContainer.isAllItemChecked ? Qt.Checked : Qt.Unchecked;

                    visible: tableContainer.checkable && model.index === 0 && elementsListObj.count > 0 && tableContainer.canSelectAll;

                    isActive: !tableContainer.readOnly;

                    onClicked: {
                        if (tableContainer.readOnly){
                            return;
                        }

                        if (checkBox.checkState === Qt.Checked){
                            tableContainer.uncheckAll();
                        }
                        else{
                            tableContainer.checkAll();
                        }
                    }
                }

                Text {
                    id: name;

                    anchors.verticalCenter: mainRec.verticalCenter;
                    anchors.left: checkBox.visible ? checkBox.right : mainRec.left;
                    anchors.right: iconSort.visible ? iconSort.left : mainRec.right;
                    anchors.leftMargin: tableContainer.textMarginHor;
                    anchors.rightMargin: iconSort.visible ? 0 : tableContainer.textMarginHor;

                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: tableContainer.emptyDecorHeader ? Text.AlignLeft :
                                                                           tableContainer.headerDecorator.isValidData("TextPosition", model.index) ?
                                                                               tableContainer.headerDecorator.getData("TextPosition", model.index) :
                                                                               Text.AlignLeft;


                    font.pixelSize: tableContainer.emptyDecorHeader ? Style.fontSize_common * deleg.scale :
                                                                      tableContainer.headerDecorator.isValidData("FontSize", model.index) ?
                                                                          tableContainer.headerDecorator.getData("FontSize", model.index) :
                                                                          Style.fontSize_common * deleg.scale;


                    font.family: Style.fontFamilyBold;

                    font.bold: tableContainer.emptyDecorHeader ? true :
                                                                 tableContainer.headerDecorator.isValidData("FontBold", model.index) ?
                                                                     tableContainer.headerDecorator.getData("FontBold", model.index) :
                                                                     true;


                    color: tableContainer.emptyDecorHeader ? Style.textColor :
                                                             tableContainer.headerDecorator.isValidData("FontColor", model.index) ?
                                                                 tableContainer.headerDecorator.getData("FontColor", model.index) :
                                                                 Style.textColor;
                    elide: tableContainer.elideMode;

                    wrapMode: tableContainer.wrapMode;

                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }

                    text: model.Name;

                    Component.onDestruction: {
                        tableContainer.heightRecalc.disconnect(name.sendHeightData);
                    }

                    Component.onCompleted: {
                        tableContainer.heightRecalc.connect(name.sendHeightData);
                    }

                    onHeightChanged: {
                        name.sendHeightData();
                    }

                    function sendHeightData(){
                        if(tableContainer.wrapMode !== Text.NoWrap){
                            if(model.index < heightModel.count){
                                var height_ = name.height +
                                        2*tableContainer.textMarginVer +
                                        topBorder.height + bottomBorder.height;

                                heightModel.setProperty(model.index, "cellHeight", height_);

                            }
                        }
                    }

                }

                Image {
                    id: iconSort;

                    anchors.verticalCenter: mainRec.verticalCenter;
                    anchors.right: mainRec.right;

                    anchors.rightMargin: 5;

                    height: 10;
                    width: visible ? 10 : 0;

                    visible: tableContainer.currentHeaderId === model.Id && tableContainer.hasSort;
                    rotation: tableContainer.currentSortOrder == "ASC" ? 180 : 0

                    sourceSize.width: width;
                    sourceSize.height: height;

                    source: tableContainer.sortIndicatorIcon
                }

                ////
                MouseArea {
                    id: headerMa;

                    anchors.fill: parent;

                    visible: tableContainer.hasSort;

                    onReleased: {
                        deleg.scale = 1;
                    }

                    onPressed: {
                        deleg.scale = 0.985;
                    }

                    onClicked: {
                        tableContainer.headerClicked(model.Id);
                    }
                }

                MouseArea{
                    id: moving;

                    anchors.right:  parent.right;
                    anchors.rightMargin: -width/2;

                    height: parent.height;
                    width: 30;

                    visible: tableContainer.canMoveColumns && model.index < headersList.count -1;
                    enabled: visible;

                    hoverEnabled: true;
                    cursorShape: pressed ?  Qt.SplitHCursor : isSplitCursor ? Qt.SplitHCursor : Qt.ArrowCursor;
                    property bool isSplitCursor: mouseX >= width/2 - splitterWidth/2 && mouseX <= width/2 + splitterWidth/2;
                    property int splitterWidth: 6;
                    property var coord: mapToItem(moving,0,0);
                    onPressed: {
                        moving.coord = mapToItem(moving,mouse.x,mouse.y)
                    }
                    onPositionChanged: {
                        if(pressed){
                            var newCoords = mapToItem(moving,mouse.x,mouse.y);
                            var deltaX = Math.trunc(newCoords.x - moving.coord.x);
                            var width_ = tableContainer.widthDecoratorDynamic.getData("Width", model.index);
                            var width_next = tableContainer.widthDecoratorDynamic.getData("Width", model.index+1);
                            var width_min = tableContainer.widthDecoratorDynamic.isValidData("MinWidth", model.index) ? tableContainer.widthDecoratorDynamic.getData("MinWidth", model.index) : tableContainer.minCellWidth;
                            var width_next_min = tableContainer.widthDecoratorDynamic.isValidData("MinWidth", model.index+1) ? tableContainer.widthDecoratorDynamic.getData("MinWidth", model.index+1) : tableContainer.minCellWidth;


                            width_ += deltaX;
                            width_next -= deltaX
                            if(width_ > width_min && width_next > width_next_min){
                                tableContainer.widthDecorator.setData("Width", width_, model.index);
                                tableContainer.widthDecorator.setData("Width", width_next, model.index+1);

                                tableContainer.setWidth();
                            }
                        }

                    }
                    onReleased: {
                        tableContainer.saveWidth();
                    }
                }

            }//delegate

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

        cacheBuffer: count * tableContainer.itemHeight;

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

        onActiveFocusChanged: {
            if (elementsListObj.activeFocus){
                tableContainer.tableSelection.subscribeEvents();
            }
            else{
                tableContainer.tableSelection.unsubscribeEvents();
            }
        }

        onContentYChanged: {
            if(tableContainer.isFrameScrolling){
                elementsBg.contentY = elementsListObj.contentY;
            }
        }

        onCountChanged: {
            console.log("onCountChanged");
        }

        delegate:
            TableDelegate {

            id: tableDelegate;

            height: visible ? tableContainer.itemHeight : 0;
            width: elementsListObj.width;
            minHeight: tableContainer.itemHeight;
            headers: tableContainer.headers;

            tableItem: tableContainer;

            readOnly: tableContainer.readOnly;

            selected: tableContainer.tableSelection.selectedIndexes.includes(model.index)
            enabled: tableContainer.properties.itemIsEnabled(model.index);
            checkedState: tableContainer.getCheckedItems().includes(model.index) ? Qt.Checked : Qt.Unchecked;

            onCheckedStateChanged: {
                tableContainer.isAllItemChecked = tableContainer.isAllChecked();
            }

            cellDecorator: tableContainer.cellDecorator;
            widthDecorator: tableContainer.widthDecorator;

            borderColorHorizontal: tableContainer.borderColorHorizontal_deleg;
            borderColorVertical: tableContainer.borderColorVertical_deleg;
            horizontalBorderSize: tableContainer.horizontalBorderSize_deleg;
            verticalBorderSize: tableContainer.verticalBorderSize_deleg;

            visibleLeftBorderFirst: tableContainer.visibleLeftBorderFirst_deleg;
            visibleRightBorderLast: tableContainer.visibleRightBorderLast_deleg;
            visibleTopBorderFirst: tableContainer.visibleTopBorderFirst_deleg;
            visibleBottomBorderLast: tableContainer.visibleBottomBorderLast_deleg;

            canSetBorderParams: tableContainer.canSetBorderParams_deleg;
            wrapMode:  tableContainer.wrapMode_deleg;
            elideMode: tableContainer.elideMode_deleg;
            isRightBorder: tableContainer.isRightBorder_deleg;

            textMarginHor: tableContainer.textMarginHor_deleg;
            textMarginVer: tableContainer.textMarginVer_deleg;

            //!!!

            Component.onCompleted: {
                tableContainer.tableSelection.selectionChanged.connect(tableDelegate.selectionChanged);
                tableContainer.checkedItemsChanged.connect(tableDelegate.checkedItemsChanged);

                tableContainer.properties.visibleItemsChanged.connect(tableDelegate.visibleItemsChanged);
                tableContainer.properties.stateItemsChanged.connect(tableDelegate.enabledItemsChanged);
                mouseArea.visible = tableContainer.hoverEnabled
            }

            Component.onDestruction: {
                if (tableContainer){
                    tableContainer.tableSelection.selectionChanged.disconnect(tableDelegate.selectionChanged);
                    tableContainer.checkedItemsChanged.disconnect(tableDelegate.checkedItemsChanged);

                    tableContainer.properties.visibleItemsChanged.disconnect(tableDelegate.visibleItemsChanged);
                    tableContainer.properties.stateItemsChanged.disconnect(tableDelegate.enabledItemsChanged);
                }
            }

            function selectionChanged(){
                tableDelegate.selected = tableContainer.tableSelection.selectedIndexes.includes(model.index);
            }

            function checkedItemsChanged(){
                tableDelegate.checkedState = tableContainer.getCheckedItems().includes(model.index) ? Qt.Checked : Qt.Unchecked;
            }

            function visibleItemsChanged(){
                tableDelegate.visible = tableContainer.properties.itemIsVisible(model.index);
            }

            function enabledItemsChanged(){
                tableDelegate.enabled = tableContainer.properties.itemIsEnabled(model.index);

                tableDelegate.readOnly = !tableDelegate.enabled;
            }

            onClicked: {
                console.log("tableContainer onClicked")

                if (!tableContainer.selectable){
                    return;
                }

                tableContainer.tableSelection.singleSelect(model.index);

                console.log("tableContainer.tableSelection", tableContainer.tableSelection.selectedIndexes)
                elementsListObj.forceActiveFocus();
            }

            onRightButtonMouseClicked: {
                console.log("onRightButtonMouseClicked")
                var point = mapToItem(null, mX, mY);
                tableContainer.rightButtonMouseClicked(point.x, point.y);
            }

            onDoubleClicked: {
                var point = mapToItem(null, mX, mY);
                tableContainer.doubleClicked(model.Id, model.index)
            }
        }
    }//Elements ListView
}
