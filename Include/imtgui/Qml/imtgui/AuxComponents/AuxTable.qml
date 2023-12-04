import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

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

    property SortController sortController: null;

    property alias delegate: elementsListObj.delegate;
    property alias elements: elementsListObj.model;

    property alias elementsList: elementsListObj;
    property alias cacheBuffer: elementsListObj.cacheBuffer;
    property alias contentHeight: elementsListObj.contentHeight;

    property alias headerDelegate: headersList.delegate;
    property real headerElementWidth: (headersList.width)/headersList.count;
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
    property int minCellWidth : 30;

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

    property bool canSetBorderParams_deleg: false;
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

    signal selectItem(string idSelected, string name);
    signal rightButtonMouseClicked(int mouseX, int mouseY);
    signal doubleClicked(int mouseX, int mouseY);
    signal setActiveFocusFromTable();
    signal headerClicked(string headerId);
    signal textFilterChanged(string id, int index, string text);
    signal filterClicked();

    signal widthRecalc();
    signal heightRecalc();

    Component.onCompleted: {
        tableContainer.headerMinHeight = tableContainer.headerHeight;
        tableContainer.setWidth();

        tableContainer.tableSelection.table = elementsListObj;
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


    }

    function setDecorators(){
        console.log("onTableDecoratorChanged");

        tableContainer.headerDecorator = tableContainer.tableDecorator.GetTreeItemModel("Headers");
        tableContainer.cellDecorator = tableContainer.tableDecorator.GetTreeItemModel("Cells");
        tableContainer.widthDecorator = tableContainer.tableDecorator.GetTreeItemModel("CellWidth");

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
        var headersCount = tableContainer.headers.GetItemsCount();

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
                widthPercent_ = widthPercent_ < 0 ? 0 : widthPercent_*tableContainer.width/100;
                lengthMinus += Math.max(width_,widthPercent_);
            }
        }

        if((tableContainer.width - lengthMinus) < count_ * tableContainer.minCellWidth || count_ == tableContainer.widthDecorator.GetItemsCount() ){
            tableContainer.widthDecoratorDynamic.Clear();
            for(let ind = 0; ind < headersCount; ind++){
                let index = tableContainer.widthDecoratorDynamic.InsertNewItem();
                tableContainer.widthDecoratorDynamic.SetData("Width",tableContainer.width/headersCount,index);
            }
            tableContainer.widthRecalc();
            return;
        }

        for(let i = 0; i < tableContainer.widthDecorator.GetItemsCount(); i++){
            let width_ = tableContainer.widthDecorator.IsValidData("Width",i) ? tableContainer.widthDecorator.GetData("Width",i): -1;
            let widthPercent_ = tableContainer.widthDecorator.IsValidData("WidthPercent",i) ? tableContainer.widthDecorator.GetData("WidthPercent",i): -1;

            if(width_ < 0  && widthPercent_ < 0 ){
                if(count_){
                    tableContainer.widthDecoratorDynamic.SetData("Width",(tableContainer.width - lengthMinus)/count_,i);
                }
            }
            else if(width_ < 0  && widthPercent_ >= 0){
                tableContainer.widthDecoratorDynamic.SetData("Width", widthPercent_*tableContainer.width/100,i);
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
        console.log("getSelectedId");
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
            property bool compl: false;

            property int currentIndex: 0;

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
                    var widthFromModel = tableContainer.widthDecoratorDynamic.IsValidData("Width", model.index) ? tableContainer.widthDecoratorDynamic.GetData("Width", model.index) : -1;


                    if(!tableContainer.widthDecoratorDynamic.GetItemsCount()){
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
                                                             tableContainer.headerDecorator.IsValidData("Color", model.index) ?
                                                                 tableContainer.headerDecorator.GetData("Color", model.index) :
                                                                 "transparent";

                    opacity:  tableContainer.emptyDecorHeader ? 1 :
                                                                tableContainer.headerDecorator.IsValidData("Opacity", model.index) ?
                                                                    tableContainer.headerDecorator.GetData("Opacity", model.index) :
                                                                    1;

                    radius: tableContainer.emptyDecorHeader ? 0 :
                                                              tableContainer.headerDecorator.IsValidData("CellRadius", model.index) ?
                                                                  tableContainer.headerDecorator.GetData("CellRadius", model.index) :0;




                    //cornerPatches
                    Rectangle{
                        id: leftTopCornerPatch;
                        anchors.left: parent.left;
                        anchors.top: parent.top;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.IsValidData("LeftTopRound", model.index) ?
                                                                       !tableContainer.headerDecorator.GetData("LeftTopRound", model.index) :true;
                    }

                    Rectangle{
                        id: rightTopCornerPatch;
                        anchors.right: parent.right;
                        anchors.top: parent.top;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.IsValidData("RightTopRound", model.index) ?
                                                                       !tableContainer.headerDecorator.GetData("RightTopRound", model.index) :true;


                    }

                    Rectangle{
                        id: leftBottomCornerPatch;
                        anchors.left: parent.left;
                        anchors.bottom: parent.bottom;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.IsValidData("LeftBottomRound", model.index) ?
                                                                       !tableContainer.headerDecorator.GetData("LeftBottomRound", model.index) :true;
                    }

                    Rectangle{
                        id: rightBottomCornerPatch;
                        anchors.right:  parent.right;
                        anchors.bottom: parent.bottom;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   tableContainer.headerDecorator.IsValidData("RightBottomRound", model.index) ?
                                                                       !tableContainer.headerDecorator.GetData("RightBottomRound", model.index) :true;
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
                                                                           tableContainer.headerDecorator.IsValidData("TextPosition", model.index) ?
                                                                               tableContainer.headerDecorator.GetData("TextPosition", model.index) :
                                                                               Text.AlignLeft;


                    font.pixelSize: tableContainer.emptyDecorHeader ? Style.fontSize_common * deleg.scale :
                                                                      tableContainer.headerDecorator.IsValidData("FontSize", model.index) ?
                                                                          tableContainer.headerDecorator.GetData("FontSize", model.index) :
                                                                          Style.fontSize_common * deleg.scale;


                    font.family: Style.fontFamilyBold;

                    font.bold: tableContainer.emptyDecorHeader ? true :
                                                                 tableContainer.headerDecorator.IsValidData("FontBold", model.index) ?
                                                                     tableContainer.headerDecorator.GetData("FontBold", model.index) :
                                                                     true;


                    color: tableContainer.emptyDecorHeader ? Style.textColor :
                                                             tableContainer.headerDecorator.IsValidData("FontColor", model.index) ?
                                                                 tableContainer.headerDecorator.GetData("FontColor", model.index) :
                                                                 Style.textColor;
                    elide: tableContainer.elideMode;

                    wrapMode: tableContainer.wrapMode;

                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }

                    text: model.Name;

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

                    visible: tableContainer.sortController && tableContainer.sortController.currentHeaderId === model.Id && tableContainer.hasSort;

                    rotation:  tableContainer.sortController && tableContainer.sortController.currentOrder == "ASC" ? 180 : 0

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
                        if (!tableContainer.sortController){
                            return;
                        }

                        if (tableContainer.sortController.currentHeaderId !== model.Id){
                            tableContainer.sortController.currentOrder = "ASC";

                            tableContainer.sortController.currentHeaderId = model.Id;
                        }
                        else{
                            tableContainer.sortController.currentOrder = tableContainer.sortController.currentOrder === "ASC" ? "DESC" : "ASC";
                        }

                        tableContainer.headerClicked(model.Id);
                    }
                }

                MouseArea{
                    id: moving;

                    anchors.right:  parent.right;

                    height: parent.height;
                    width: 20;

                    visible: tableContainer.canMoveColumns && model.index < headersList.count -1;
                    enabled: visible;

//                    hoverEnabled: true;
                    cursorShape: containsMouse ? Qt.SplitHCursor : containsPress ? Qt.PointingHandCursor : Qt.ArrowCursor;
//                    cursorShape: containsMouse ? Qt.SplitHCursor : Qt.ArrowCursor;
                    property var coord: mapToItem(moving,0,0);
                    onPressed: {
                        moving.coord = mapToItem(moving,mouse.x,mouse.y)
                    }
                    onPositionChanged: {
                        var newCoords = mapToItem(moving,mouse.x,mouse.y);
                        var deltaX = Math.trunc(newCoords.x - moving.coord.x);
                        var width_ = tableContainer.widthDecoratorDynamic.GetData("Width", model.index);
                        var width_next = tableContainer.widthDecoratorDynamic.GetData("Width", model.index+1);

                        width_ += deltaX;
                        width_next -= deltaX
                        if(width_ > tableContainer.minCellWidth && width_next > tableContainer.minCellWidth){
                            tableContainer.widthDecorator.SetData("Width", width_, model.index);
                            tableContainer.widthDecorator.SetData("Width", width_next, model.index+1);

                            tableContainer.setWidth();
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

            //TODO -> Style
            color: "lightgray";
            visible: tableContainer.emptyDecor;
        }
    }//headers

    Rectangle {
        id: elementsBg;

        anchors.fill: elementsListObj;
        color: Style.baseColor;

        radius: tableContainer.radius;
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
        targetItem: elementsListObj;
    }

    ListView {
        id: elementsListObj;

        anchors.left: parent.left;
        anchors.right: headersPanel.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;

        boundsBehavior: Flickable.StopAtBounds;

        cacheBuffer: 1000;

        clip: true;

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
                    console.log("tableContainer selectionChanged")

                    tableDelegate.selected = tableContainer.tableSelection.selectedIndexes.includes(model.index);

                    if (tableDelegate.selected){
                        //elementsListObj.positionViewAtIndex(model.index, ListView.Visible);
                    }
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
                    tableContainer.doubleClicked(point.x, point.y)

                    console.log("selectItem", model.Id, model.Name)
                    tableContainer.selectItem(model.Id, model.Name);
                }
            }
    }//Elements ListView
}
