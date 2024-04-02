import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: tableDelegateContainer;

    width: 1000;
    height: visible ? minHeight : 0;
    property real minHeight: 40;

    color: tableDelegateContainer.selected ? Style.selectedColor : "transparent";

    visible: true

    property int textTopMargin: 8;
    property int columnCount: table ? table.columnCount : 0;

    property bool selected: false;
    property int checkedState: Qt.Unchecked;

    property TableBase table: null;

    // property var bodyArray:  [];

    property var dataModel: model;

    property int rowIndex: model.index;

    // property TreeItemModel headers: TreeItemModel{};

    // property TreeItemModel cellDecorator : TreeItemModel{};
    // property TreeItemModel widthDecorator : TreeItemModel{};
    // property TreeItemModel widthDecoratorDynamic : TreeItemModel{};
    property TreeItemModel cellDecorator : table.cellDecorator

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

    property  Component cellDelegate: table ? table.cellDelegate : cellDelegateDefault;
    property  Component cellDelegateDefault: TableCellDelegate {};

    signal clicked();
    signal rightButtonMouseClicked(int mX, int mY);
    signal doubleClicked(int mX, int mY);

    signal widthRecalc();


    Component.onCompleted: {
        tableDelegateContainer.compl = true;
    }


    onCellDecoratorChanged: {
        if (tableDelegateContainer.table.cellDecorator){
            // tableDelegateContainer.emptyDecorCell = !tableDelegateContainer.table.cellDecorator.GetItemsCount();
            tableDelegateContainer.setBorderParams();
        }
    }

    // onHeadersChanged: {
    //     tableDelegateContainer.columnCount = tableDelegateContainer.headers.GetItemsCount();
    //     setCellHeightModelDefault();
    // }
    onTableChanged: {
        if (tableDelegateContainer.table && tableDelegateContainer.table.checkable){
            checkBox.createObject(tableDelegateContainer)
        }
    }

    onWidthChanged: {
        if(tableDelegateContainer.wrapMode !== Text.NoWrap){
            if(pauseHeight){
                pauseHeight.stop();
                pauseHeight.start();
            }
        }

    }

    onColumnCountChanged: {
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
        console.log("Table delegate getSelectedId", model.Id)
        return model.Id;
    }

    function getSelectedName(){
        return model.Name;
    }

    function setBorderParams(){
        // if(tableDelegateContainer.emptyDecorCell){
        //     return;
        // }
        // if(!tableDelegateContainer.canSetBorderParams){
        //     return;
        // }

        if(tableDelegateContainer.table.cellDecorator.IsValidData("BorderColorHorizontal")){
            tableDelegateContainer.borderColorHorizontal = tableDelegateContainer.table.cellDecorator.GetData("BorderColorHorizontal");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("BorderColorVertical")){
            tableDelegateContainer.borderColorVertical = tableDelegateContainer.table.cellDecorator.GetData("BorderColorVertical");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("HorizontalBorderSize")){
            tableDelegateContainer.horizontalBorderSize = tableDelegateContainer.table.cellDecorator.GetData("HorizontalBorderSize");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("VerticalBorderSize")){
            tableDelegateContainer.verticalBorderSize = tableDelegateContainer.table.cellDecorator.GetData("VerticalBorderSize");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("VisibleLeftBorderFirst")){
            tableDelegateContainer.visibleLeftBorderFirst = tableDelegateContainer.table.cellDecorator.GetData("VisibleLeftBorderFirst");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("VisibleRightBorderLast")){
            tableDelegateContainer.visibleRightBorderLast = tableDelegateContainer.table.cellDecorator.GetData("VisibleRightBorderLast");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("VisibleTopBorderFirst")){
            tableDelegateContainer.visibleTopBorderFirst = tableDelegateContainer.table.cellDecorator.GetData("VisibleTopBorderFirst");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("VisibleBottomBorderLast")){
            tableDelegateContainer.visibleBottomBorderLast = tableDelegateContainer.table.cellDecorator.GetData("VisibleBottomBorderLast");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("WrapMode")){
            tableDelegateContainer.wrapMode = tableDelegateContainer.table.cellDecorator.GetData("WrapMode");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("IsRightBorder")){
            tableDelegateContainer.isRightBorder = tableDelegateContainer.table.cellDecorator.GetData("IsRightBorder");
        }
        if(tableDelegateContainer.table.cellDecorator.IsValidData("ElideMode")){
            tableDelegateContainer.elideMode = tableDelegateContainer.table.cellDecorator.GetData("ElideMode");
        }

    }

    function setHeightModelElememt(index_,height_){
        console.log("setHeightModelElememt", index_,height_);

        if(!tableDelegateContainer.table || !tableDelegateContainer.table.canFitHeight ){
            return;
        }
        if(index_ < heightModel.count){
            heightModel.setProperty(index_, "cellHeight", height_);
        }
    }

    function setCellHeightModelDefault(){
        if(!tableDelegateContainer.table || !tableDelegateContainer.table.canFitHeight ){
            return;
        }
        for(var i = 0; i < tableDelegateContainer.columnCount; i++){
            heightModel.append({"cellHeight": tableDelegateContainer.minHeight})
        }
    }

    function setCellHeight(){
        if(!tableDelegateContainer.table || !tableDelegateContainer.table.canFitHeight ){
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




    Rectangle{
        id: alternatingRect;

        anchors.fill: parent;

        color: tableDelegateContainer.table.hoverEnabled && ma.containsMouse ? Style.selectedColor :
            tableDelegateContainer.table.enableAlternating ? tableDelegateContainer.table.alternatingColor : 'transparent';

        opacity: tableDelegateContainer.selected ? tableDelegateContainer.selectedOpacity :
                tableDelegateContainer.table.hoverEnabled && ma.containsMouse ? tableDelegateContainer.hoverOpacity :
                tableDelegateContainer.table.enableAlternating && model.index % 2 === 0 ? tableDelegateContainer.table.alternatingOpacity: 0;

        visible: !tableDelegateContainer.selected && tableDelegateContainer.table.selectable;
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
                console.log("onClicked2", model["Id"])
                if (mouse.button === Qt.RightButton) {
                    tableDelegateContainer.rightButtonMouseClicked(this.mouseX, this.mouseY);
                }
                tableDelegateContainer.clicked();

                timer.start();
            }

            if (ma.clickCount == 2){
                console.log("onDoubleClicked2")
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
        CheckBox {
            id: checkBox;

            z: 1000;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            checkState: tableDelegateContainer.checkedState;

            visible: tableDelegateContainer.table ? tableDelegateContainer.table.checkable : false;
            isActive: !tableDelegateContainer.readOnly;

            onClicked: {
                if (tableDelegateContainer.readOnly){
                    return;
                }

                if (tableDelegateContainer.table.itemIsChecked(tableDelegateContainer.rowIndex)){
                    tableDelegateContainer.table.uncheckItem(tableDelegateContainer.rowIndex);
                }
                else{
                    tableDelegateContainer.table.checkItem(tableDelegateContainer.rowIndex);
                }
            }
        }
    }
}
