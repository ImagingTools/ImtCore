import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
    id: tableDelegateContainer;

	width: Style.sizeHintXXXL;
    height: visible ? minHeight : 0;
	color: tableDelegateContainer.selected ? Style.selectedColor : "transparent";

	property int textTopMargin: Style.marginS;
    property int count: 0;

	property real minHeight: Style.controlHeightL;

    property bool selected: false;
    property int checkedState: Qt.Unchecked;

    property Item tableItem: null;

    property var bodyArray:  [];

	property var dataModel: model;

    property int rowIndex: model.index;

	property TreeItemModel headers: TreeItemModel{};

    property TreeItemModel cellDecorator : TreeItemModel{};
    property TreeItemModel widthDecorator : TreeItemModel{};
    property TreeItemModel widthDecoratorDynamic : TreeItemModel{};

    property alias mouseArea: ma;

    property bool emptyDecorCell: true;

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

    property bool canSetBorderParams: false;
    property int wrapMode: Text.NoWrap;
    property int elideMode: Text.ElideRight;
    property bool isRightBorder: false;
    //

    property string maxLengthText: '';

	property int textMarginHor: Style.marginS;
	property int textLeftMargin: Style.marginS;
	property int textRightMargin: Style.marginS;
    property int textMarginVer: textTopMargin;

    property bool compl: false;

    property string selectedColor: Style.selectedColor;

    property  Component cellDelegate: tableItem ? tableItem.cellDelegate : null//: cellDelegateDefault;
//    property  Component cellDelegateDefault: TableCellDelegate {};

    signal clicked();
    signal rightButtonMouseClicked(int mX, int mY);
    signal doubleClicked(int mX, int mY);

    signal widthRecalc();


    Component.onCompleted: {
        tableDelegateContainer.compl = true;
        tableDelegateContainer.minHeight = tableDelegateContainer.height;

    }



    onCellDecoratorChanged: {
        if (tableDelegateContainer.cellDecorator){
            tableDelegateContainer.emptyDecorCell = !tableDelegateContainer.cellDecorator.getItemsCount();
            tableDelegateContainer.setBorderParams();
        }
    }

    onHeadersChanged: {
        tableDelegateContainer.count = tableDelegateContainer.headers.getItemsCount();
        setCellHeightModelDefault();
    }

    onWidthChanged: {
        if(tableDelegateContainer.wrapMode !== Text.NoWrap){
            if(pauseHeight){
                pauseHeight.stop();
                pauseHeight.start();
            }
        }

    }

    onCountChanged: {
        if(tableDelegateContainer.wrapMode !== Text.NoWrap){
            heightModel.append({"cellHeight": 0});
            if(pauseHeight){
                pauseHeight.stop();
                pauseHeight.start();
            }
        }
    }

    function getItemData(){
        return model;
    }

    function getSelectedId(){
        return model.id || model.Id || "";
    }

    function getSelectedName(){
        return model.name;
    }

    function setBorderParams(){
        if(tableDelegateContainer.emptyDecorCell){
            return;
        }
        if(!tableDelegateContainer.canSetBorderParams){
            return;
        }

        if(tableDelegateContainer.cellDecorator.isValidData("BorderColorHorizontal")){
            tableDelegateContainer.borderColorHorizontal = tableDelegateContainer.cellDecorator.getData("BorderColorHorizontal");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("BorderColorVertical")){
            tableDelegateContainer.borderColorVertical = tableDelegateContainer.cellDecorator.getData("BorderColorVertical");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("HorizontalBorderSize")){
            tableDelegateContainer.horizontalBorderSize = tableDelegateContainer.cellDecorator.getData("HorizontalBorderSize");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("VerticalBorderSize")){
            tableDelegateContainer.verticalBorderSize = tableDelegateContainer.cellDecorator.getData("VerticalBorderSize");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("VisibleLeftBorderFirst")){
            tableDelegateContainer.visibleLeftBorderFirst = tableDelegateContainer.cellDecorator.getData("VisibleLeftBorderFirst");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("VisibleRightBorderLast")){
            tableDelegateContainer.visibleRightBorderLast = tableDelegateContainer.cellDecorator.getData("VisibleRightBorderLast");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("VisibleTopBorderFirst")){
            tableDelegateContainer.visibleTopBorderFirst = tableDelegateContainer.cellDecorator.getData("VisibleTopBorderFirst");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("VisibleBottomBorderLast")){
            tableDelegateContainer.visibleBottomBorderLast = tableDelegateContainer.cellDecorator.getData("VisibleBottomBorderLast");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("WrapMode")){
            tableDelegateContainer.wrapMode = tableDelegateContainer.cellDecorator.getData("WrapMode");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("IsRightBorder")){
            tableDelegateContainer.isRightBorder = tableDelegateContainer.cellDecorator.getData("IsRightBorder");
        }
        if(tableDelegateContainer.cellDecorator.isValidData("ElideMode")){
            tableDelegateContainer.elideMode = tableDelegateContainer.cellDecorator.getData("ElideMode");
        }

    }

    function setHeightModelElememt(index_,height_){
        if(!tableDelegateContainer.tableItem || !tableDelegateContainer.tableItem.canFitHeight ){
            return;
        }
        if(index_ < heightModel.count){
            heightModel.setProperty(index_, "cellHeight", height_);
        }
    }

    function setCellHeightModelDefault(){
        if(!tableDelegateContainer.tableItem || !tableDelegateContainer.tableItem.canFitHeight ){
            return;
        }
        for(var i = 0; i < tableDelegateContainer.count; i++){
            heightModel.append({"cellHeight": tableDelegateContainer.minHeight})
        }
    }

    function setCellHeight(){
        if(!tableDelegateContainer.tableItem || !tableDelegateContainer.tableItem.canFitHeight ){
            return;
        }
        if(tableDelegateContainer.wrapMode == Text.NoWrap){
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

    CheckBox {
        id: checkBox;

        z: 1000;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        checkState: tableDelegateContainer.checkedState;

        visible: tableDelegateContainer.tableItem ? tableDelegateContainer.tableItem.checkable : false;
        isActive: !tableDelegateContainer.readOnly;

		function nextCheckState(){
            if (tableDelegateContainer.readOnly){
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

    ListView {
        id: dataList;

        anchors.left: checkBox.visible ? checkBox.right : parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        clip: true;
        orientation: ListView.Horizontal;

        boundsBehavior: Flickable.StopAtBounds;

        spacing: 0;
        property bool compl: false;
        Component.onCompleted: {
            dataList.compl = true;
        }
        model: tableDelegateContainer.count;

        delegate: tableDelegateContainer.cellDelegate;
    }//dataList

    Rectangle{
        id: alternatingRect;

        anchors.fill: parent;

        color: tableDelegateContainer.tableItem.hoverEnabled && ma.containsMouse ? Style.selectedColor :
            tableDelegateContainer.tableItem.enableAlternating ? tableDelegateContainer.tableItem.alternatingColor : 'transparent';

        opacity: tableDelegateContainer.selected ? tableDelegateContainer.selectedOpacity :
                tableDelegateContainer.tableItem.hoverEnabled && ma.containsMouse ? tableDelegateContainer.hoverOpacity :
                tableDelegateContainer.tableItem.enableAlternating && model.index % 2 === 0 ? tableDelegateContainer.tableItem.alternatingOpacity: 0;

        visible: !tableDelegateContainer.selected && tableDelegateContainer.tableItem.selectable;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;
        hoverEnabled: true;

        propagateComposedEvents: true;

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
                if (mouse.button === Qt.RightButton){
                    tableDelegateContainer.rightButtonMouseClicked(this.mouseX, this.mouseY);
                }
                tableDelegateContainer.clicked();

                timer.start();
            }

            if (ma.clickCount == 2){
                if (mouse.button === Qt.RightButton){
                    return;
                }

                tableDelegateContainer.doubleClicked(this.mouseX, this.mouseY);

                ma.clickCount = 0;
                timer.stop();
            }

            mouse.accepted = false;
        }
    }
}
