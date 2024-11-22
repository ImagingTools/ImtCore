import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: tableDelegateContainer;

    width: 1000;
    height: visible ? minHeight : 0;
    property real minHeight: 40;
    property real contentHeight: height;

    color: tableDelegateContainer.selected ? Style.selectedColor : "transparent";

    visible: true

    property int textTopMargin: 8;
    property int columnCount: tableItem ? tableItem.columnCount : 0;

    property bool selected: false;
    property int checkedState: Qt.Unchecked;

    property TableBase tableItem: null;

    property var dataModel: model;

    property int rowIndex: model.index;

    property TreeItemModel cellDecorator : tableItem.cellDecorator

    property alias mouseArea: ma;

    property bool readOnly: false;

    property bool enabled: true;

    property real selectedOpacity: ((Style.selectedOpacity !== undefined && Style.selectedOpacity !== null) ?  Style.selectedOpacity :  0.5);
    property real hoverOpacity:((Style.hoverOpacity !== undefined && Style.hoverOpacity !== null) ?  Style.hoverOpacity :  selectedOpacity/2);

    //
    property string borderColorHorizontal: "transparent";
    property string borderColorVertical: "transparent";
    property int horizontalBorderSize: 0;
    property int verticalBorderSize: 0;

    property bool visibleLeftBorderFirst: true;
    property bool visibleRightBorderLast: false;
    property bool visibleTopBorderFirst: false;
    property bool visibleBottomBorderLast: true;

    property bool canSetBorderParams: tableItem ? tableItem.canSetBorderParams : false;
    property int wrapMode: tableItem ? tableItem.wrapMode_deleg : Text.NoWrap;
    property int elideMode: Text.ElideRight;
    property bool isRightBorder: false;
    //

    property string maxLengthText: '';

    property string cellColor: "transparent";

    property int textMarginHor: 8;
    property int textLeftMargin: 8;
    property int textRightMargin: 8;
    property int textMarginVer: textTopMargin;

    property bool compl: false;

    property string selectedColor: Style.selectedColor;

    property  Component cellDelegate: tableItem ? tableItem.cellDelegate : cellDelegateDefault;
    property  Component cellDelegateDefault: TableCellDelegate {};

    signal clicked();
    signal rightButtonMouseClicked(int mX, int mY);
    signal doubleClicked(int mX, int mY);
    signal widthRecalc();

    Component.onCompleted: {
        tableDelegateContainer.compl = true;
    }

    Component.onDestruction: {
        if (tableItem){
            tableItem.selectionChanged.disconnect(tableDelegateContainer.onSelectionChanged);
            tableItem.elementsChanged.disconnect(tableDelegateContainer.onElementsChanged);
            tableItem.checkedItemsChanged.disconnect(tableDelegateContainer.checkedItemsChanged);
            tableItem.properties.visibleItemsChanged.disconnect(tableDelegateContainer.visibleItemsChanged);
            tableItem.properties.stateItemsChanged.disconnect(tableDelegateContainer.enabledItemsChanged);
        }
    }

    onCellDecoratorChanged: {
        if (tableDelegateContainer.tableItem.cellDecorator){
            tableDelegateContainer.setBorderParams();
        }
    }

    onClicked: {
        if (!tableItem){
            console.error("tableItem is invalid")

            return;
        }

        tableItem.clicked(rowIndex);

        if (!tableItem.selectable){
            return;
        }

        tableItem.tableSelection.singleSelect(model.index);

        tableItem.forceActiveFocus();
    }

    onDoubleClicked: {
        if (tableItem){
            tableItem.doubleClicked(model.Id, model.index)
        }
    }

    onRightButtonMouseClicked: {
        var point = mapToItem(null, mX, mY);

        if (tableItem){
            tableItem.rightButtonMouseClicked(point.x, point.y);
        }
    }

    onCheckedStateChanged: {
        if (tableDelegateContainer.tableItem){
            tableDelegateContainer.tableItem.isAllItemChecked = tableDelegateContainer.tableItem.isAllChecked();
        }
    }

    onTableItemChanged: {
        if (tableItem){
            checkedItemsChanged()
            tableItem.checkedItemsChanged.connect(tableDelegateContainer.checkedItemsChanged);
            tableItem.selectionChanged.connect(tableDelegateContainer.onSelectionChanged);
            tableItem.elementsChanged.connect(tableDelegateContainer.onElementsChanged);
            tableItem.properties.visibleItemsChanged.connect(tableDelegateContainer.visibleItemsChanged);
            tableItem.properties.stateItemsChanged.connect(tableDelegateContainer.enabledItemsChanged);
            ma.visible = tableItem.hoverEnabled
            if (tableItem.checkable){
                checkBox.createObject(tableDelegateContainer)
            }
        }
    }

    onColumnCountChanged: {
        if(tableDelegateContainer.tableItem.wrapMode_deleg !== Text.NoWrap){
            setCellHeightModelDefault();
            if(pauseHeight){
                pauseHeight.restart()
            }
        }
    }

    function onSelectionChanged(selection){
        if (!tableDelegateContainer.tableItem){
            return;
        }

        tableDelegateContainer.selected = selection.includes(tableDelegateContainer.rowIndex);
    }

    function onElementsChanged(){
        if (!tableDelegateContainer.tableItem){
            return;
        }

        let selection = tableDelegateContainer.tableItem.tableSelection.selectedIndexes;

        tableDelegateContainer.selected = selection.includes(tableDelegateContainer.rowIndex);
    }

    function getItemData(){
        return model;
    }

    function getSelectedId(){
        return model.Id;
    }

    function getSelectedName(){
        return model.Name;
    }

    function setBorderParams(){
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("BorderColorHorizontal")){
            tableDelegateContainer.borderColorHorizontal = tableDelegateContainer.tableItem.cellDecorator.getData("BorderColorHorizontal");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("BorderColorVertical")){
            tableDelegateContainer.borderColorVertical = tableDelegateContainer.tableItem.cellDecorator.getData("BorderColorVertical");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("HorizontalBorderSize")){
            tableDelegateContainer.horizontalBorderSize = tableDelegateContainer.tableItem.cellDecorator.getData("HorizontalBorderSize");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("VerticalBorderSize")){
            tableDelegateContainer.verticalBorderSize = tableDelegateContainer.tableItem.cellDecorator.getData("VerticalBorderSize");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("VisibleLeftBorderFirst")){
            tableDelegateContainer.visibleLeftBorderFirst = tableDelegateContainer.tableItem.cellDecorator.getData("VisibleLeftBorderFirst");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("VisibleRightBorderLast")){
            tableDelegateContainer.visibleRightBorderLast = tableDelegateContainer.tableItem.cellDecorator.getData("VisibleRightBorderLast");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("VisibleTopBorderFirst")){
            tableDelegateContainer.visibleTopBorderFirst = tableDelegateContainer.tableItem.cellDecorator.getData("VisibleTopBorderFirst");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("VisibleBottomBorderLast")){
            tableDelegateContainer.visibleBottomBorderLast = tableDelegateContainer.tableItem.cellDecorator.getData("VisibleBottomBorderLast");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("WrapMode")){
            tableDelegateContainer.wrapMode = tableDelegateContainer.tableItem.cellDecorator.getData("WrapMode");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("IsRightBorder")){
            tableDelegateContainer.isRightBorder = tableDelegateContainer.tableItem.cellDecorator.getData("IsRightBorder");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("ElideMode")){
            tableDelegateContainer.elideMode = tableDelegateContainer.tableItem.cellDecorator.getData("ElideMode");
        }
        if(tableDelegateContainer.tableItem.cellDecorator.isValidData("Color")){
            tableDelegateContainer.cellColor = tableDelegateContainer.tableItem.cellDecorator.getData("Color");
        }
    }

    function setHeightModelElememt(index_,height_){
        if(!tableDelegateContainer.tableItem || !tableDelegateContainer.tableItem.canFitHeight ){
            return;
        }
        if(tableDelegateContainer.tableItem.wrapMode_deleg == Text.NoWrap){
            return;
        }

        if(index_ < heightModel.count){
            heightModel.setProperty(index_, "cellHeight", height_);
        }
        if(pauseHeight){
            pauseHeight.restart();
        }
    }

    function setCellHeightModelDefault(){
        if(!tableDelegateContainer.tableItem || !tableDelegateContainer.tableItem.canFitHeight ){
            return;
        }
        heightModel.clear();
        for(var i = 0; i < tableDelegateContainer.columnCount; i++){
            heightModel.append({"cellHeight": tableDelegateContainer.minHeight})
        }
    }

    function setCellHeight(){
        if(!tableDelegateContainer.tableItem || !tableDelegateContainer.tableItem.canFitHeight ){
            return;
        }
        if(tableDelegateContainer.tableItem.wrapMode_deleg == Text.NoWrap){
            return;
        }

        var maxVal = 0;
        for(var i = 0; i < heightModel.count; i++){
            var currVal = heightModel.get(i).cellHeight;
            if(currVal > maxVal){
                maxVal = currVal;
            }
        }
        tableDelegateContainer.height = Math.max(maxVal, tableDelegateContainer.minHeight);
    }

    function checkedItemsChanged(){
        if (tableItem && model){
            checkedState = tableItem.getCheckedItems().includes(model.index) ? Qt.Checked : Qt.Unchecked;
        }
    }

    function visibleItemsChanged(){
        if (tableItem && model){
            visible = tableItem.properties.itemIsVisible(model.index);
        }
    }

    function enabledItemsChanged(){
        if (tableItem && model){
            enabled = tableItem.properties.itemIsEnabled(model.index);

            readOnly = !enabled;
        }
    }

    PauseAnimation {
        id: pauseHeight;

        duration: 100;
        onFinished: {
            tableDelegateContainer.setCellHeight();
        }
    }

    ListModel{
        id: heightModel;
    }

    Rectangle{
        id: alternatingRect;

        anchors.fill: parent;

        property bool containsMouse: tableDelegateContainer.tableItem.hoverEnabled && ma.containsMouse;

        color: containsMouse ? Style.selectedColor :
            tableDelegateContainer.tableItem.enableAlternating ? tableDelegateContainer.tableItem.alternatingColor : 'transparent';

        opacity: tableDelegateContainer.selected ? tableDelegateContainer.selectedOpacity :
                containsMouse ? tableDelegateContainer.hoverOpacity :
                tableDelegateContainer.tableItem.enableAlternating && model.index % 2 === 0 ? tableDelegateContainer.tableItem.alternatingOpacity: 0;

        visible: !tableDelegateContainer.selected && (tableDelegateContainer.tableItem.enableAlternating || tableDelegateContainer.tableItem.selectable);
        z: 10;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;
        hoverEnabled: true;

        acceptedButtons: Qt.LeftButton | Qt.RightButton;

        Timer{
            id:timer
            interval: 300
            onTriggered: {
                ma.clickCount = 0;
            }
        }

        property int clickCount: 0;
        onClicked: {
            clickCount++;

            if (ma.clickCount == 1){
                if (mouse.button === Qt.RightButton) {
                    tableDelegateContainer.rightButtonMouseClicked(this.mouseX, this.mouseY);
                }
                tableDelegateContainer.clicked();

                timer.start();
            }

            if (ma.clickCount == 2){
                if (mouse.button === Qt.RightButton) {
                    return;
                }

                tableDelegateContainer.doubleClicked(this.mouseX, this.mouseY);

                ma.clickCount = 0;
                timer.stop();
            }

            mouse.accepted = false;
        }
    }

    Component{
        id: checkBox;

        CheckBox {

            z: 1000;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: Style.size_mainMargin;

            checkState: tableDelegateContainer.checkedState;

            visible: tableDelegateContainer.tableItem ? tableDelegateContainer.tableItem.checkable : false;
            isActive: !tableDelegateContainer.readOnly;

            onClicked: {
                if (tableDelegateContainer.readOnly){
                    return;
                }

                if (!tableDelegateContainer.tableItem){
                    return;
                }

                if (tableDelegateContainer.tableItem.itemIsChecked(tableDelegateContainer.rowIndex)){
                    tableDelegateContainer.tableItem.uncheckItem(tableDelegateContainer.rowIndex);
                }
                else{
                    tableDelegateContainer.tableItem.checkItem(tableDelegateContainer.rowIndex);
                }
            }
        }
    }
}
