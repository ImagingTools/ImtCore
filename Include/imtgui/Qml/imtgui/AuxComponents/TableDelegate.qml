import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: tableDelegateContainer;

    width: 1000;
    height: visible ? minHeight : 0;
	color: tableDelegateContainer.selected ? Style.selectedColor : "transparent";

    property int textTopMargin: 8;
    property int count: 0;

    property real minHeight: 40;

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
        tableDelegateContainer.minHeight = tableDelegateContainer.height;

    }



    onCellDecoratorChanged: {
        if (tableDelegateContainer.cellDecorator){
            tableDelegateContainer.emptyDecorCell = !tableDelegateContainer.cellDecorator.GetItemsCount();
            tableDelegateContainer.setBorderParams();
        }
    }

    onHeadersChanged: {
        tableDelegateContainer.count = tableDelegateContainer.headers.GetItemsCount();
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
        return model.Id;
    }

    function getSelectedName(){
        return model.Name;
    }

    function setBorderParams(){
        if(tableDelegateContainer.emptyDecorCell){
            return;
        }
        if(!tableDelegateContainer.canSetBorderParams){
            return;
        }

        if(tableDelegateContainer.cellDecorator.IsValidData("BorderColorHorizontal")){
            tableDelegateContainer.borderColorHorizontal = tableDelegateContainer.cellDecorator.GetData("BorderColorHorizontal");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("BorderColorVertical")){
            tableDelegateContainer.borderColorVertical = tableDelegateContainer.cellDecorator.GetData("BorderColorVertical");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("HorizontalBorderSize")){
            tableDelegateContainer.horizontalBorderSize = tableDelegateContainer.cellDecorator.GetData("HorizontalBorderSize");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("VerticalBorderSize")){
            tableDelegateContainer.verticalBorderSize = tableDelegateContainer.cellDecorator.GetData("VerticalBorderSize");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("VisibleLeftBorderFirst")){
            tableDelegateContainer.visibleLeftBorderFirst = tableDelegateContainer.cellDecorator.GetData("VisibleLeftBorderFirst");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("VisibleRightBorderLast")){
            tableDelegateContainer.visibleRightBorderLast = tableDelegateContainer.cellDecorator.GetData("VisibleRightBorderLast");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("VisibleTopBorderFirst")){
            tableDelegateContainer.visibleTopBorderFirst = tableDelegateContainer.cellDecorator.GetData("VisibleTopBorderFirst");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("VisibleBottomBorderLast")){
            tableDelegateContainer.visibleBottomBorderLast = tableDelegateContainer.cellDecorator.GetData("VisibleBottomBorderLast");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("WrapMode")){
            tableDelegateContainer.wrapMode = tableDelegateContainer.cellDecorator.GetData("WrapMode");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("IsRightBorder")){
            tableDelegateContainer.isRightBorder = tableDelegateContainer.cellDecorator.GetData("IsRightBorder");
        }
        if(tableDelegateContainer.cellDecorator.IsValidData("ElideMode")){
            tableDelegateContainer.elideMode = tableDelegateContainer.cellDecorator.GetData("ElideMode");
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

        onClicked: {
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

        visible: !tableDelegateContainer.selected;
    }

    MouseArea {
        id: ma;

        z: 1;

		anchors.fill: parent;
        hoverEnabled: true

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

            if ( ma.clickCount == 1){
                console.log("onClicked", model["Id"])
                if (mouse.button === Qt.RightButton) {
                    tableDelegateContainer.rightButtonMouseClicked(this.mouseX, this.mouseY);
                }
                tableDelegateContainer.clicked();

                timer.start();
            }

            if ( ma.clickCount == 2){
                console.log("onDoubleClicked")
                if (mouse.button === Qt.RightButton) {
                    return;
                }

                tableDelegateContainer.doubleClicked(this.mouseX, this.mouseY);

                ma.clickCount = 0;
                timer.stop();
            }
        }
//        onDoubleClicked: {
//            console.log("onDoubleClicked")
//            if (mouse.button === Qt.RightButton) {
//                return;
//            }

//            tableDelegateContainer.doubleClicked(this.mouseX, this.mouseY);
//        }
    }
}
