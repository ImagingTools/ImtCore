import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: tableDelegateContainer;

    width: 1000;
    height: minHeight;
	color: "transparent";

    property var cellDelegate;
    property int textTopMargin: 8;
    property int count: 0; // bodyArray.length;

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

    property bool emptyDecorCell: true;

    property string selectedColor: Style.selectedColor;
    property real selectedOpacity: 0.5;

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

    signal clicked();
    signal rightButtonMouseClicked(int mX, int mY);
    signal doubleClicked(int mX, int mY);

    signal widthRecalc();

    Component.onCompleted: {
        tableDelegateContainer.compl = true;
        tableDelegateContainer.minHeight = tableDelegateContainer.height;
        tableDelegateContainer.setWidth();
    }

    onWidthDecoratorChanged: {
        tableDelegateContainer.setWidth();
    }

//    onCellDelegateChanged: {
//        dataList.delegate = tableDelegateContainer.cellDelegate;
//    }

    onCellDecoratorChanged: {
        tableDelegateContainer.emptyDecorCell = !tableDelegateContainer.cellDecorator.GetItemsCount();
        tableDelegateContainer.setBorderParams();
    }

    onHeadersChanged: {
        tableDelegateContainer.count = tableDelegateContainer.headers.GetItemsCount();
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

    function setWidth(){

        tableDelegateContainer.widthDecoratorDynamic.Clear();
        tableDelegateContainer.widthDecoratorDynamic.Copy(tableDelegateContainer.widthDecorator);

        if(!tableDelegateContainer.widthDecorator.GetItemsCount()){
            tableDelegateContainer.widthRecalc();
            return;
        }

        var count_ = 0;
        var lengthMinus = 0;

        for(var i = 0; i < tableDelegateContainer.widthDecorator.GetItemsCount(); i++){

            var width_ = tableDelegateContainer.widthDecorator.IsValidData("Width",i) ? tableDelegateContainer.widthDecorator.GetData("Width",i): -1;
            var widthPercent_ = tableDelegateContainer.widthDecorator.IsValidData("WidthPercent",i) ? tableDelegateContainer.widthDecorator.GetData("WidthPercent",i): -1;

            if((width_ == -1) && (widthPercent_ == -1)){
                count_++;
            }
            else{
                width_ = width_< 0 ? 0 : width_;
                widthPercent_ = widthPercent_ < 0 ? 0 : widthPercent_*tableDelegateContainer.width/100;
                lengthMinus += Math.max(width_,widthPercent_);
            }
        }

        if((tableDelegateContainer.width - lengthMinus) < 0 || count_ == tableDelegateContainer.widthDecorator.GetItemsCount() ){
            tableDelegateContainer.widthDecoratorDynamic.Clear();
            tableDelegateContainer.widthRecalc();
            return;
        }

        for(var i = 0; i < tableDelegateContainer.widthDecorator.GetItemsCount(); i++){

            var width_ = tableDelegateContainer.widthDecorator.IsValidData("Width",i) ? tableDelegateContainer.widthDecorator.GetData("Width",i): -1;
            var widthPercent_ = tableDelegateContainer.widthDecorator.IsValidData("WidthPercent",i) ? tableDelegateContainer.widthDecorator.GetData("WidthPercent",i): -1;

            if(width_ < 0  && widthPercent_ < 0 ){
                if(count_){
                    tableDelegateContainer.widthDecoratorDynamic.SetData("Width",(tableDelegateContainer.width - lengthMinus)/count_,i);
                }
            }
            else if(width_ < 0  && widthPercent_ >= 0){
                tableDelegateContainer.widthDecoratorDynamic.SetData("Width", widthPercent_*tableDelegateContainer.width/100,i);
            }

            width_ = tableDelegateContainer.widthDecoratorDynamic.IsValidData("Width",i) ? tableDelegateContainer.widthDecoratorDynamic.GetData("Width",i): -1;

            if(width_ < 0){
                if(count_){
                    tableDelegateContainer.widthDecoratorDynamic.SetData("Width",(tableDelegateContainer.width - lengthMinus)/count_,i);
                }

            }

        }

        tableDelegateContainer.widthRecalc();
    }

    onWidthChanged: {
        tableDelegateContainer.setWidth();
        if(tableDelegateContainer.wrapMode !== Text.NoWrap){
            pause.stop();
            pause.start();
        }

    }

    onCountChanged: {
        if(tableDelegateContainer.wrapMode !== Text.NoWrap){
            heightModel.append({"cellHeight": 0});
            pause.stop();
            pause.start();
        }
    }

    function setCellHeight(){
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
        id: pause;
        duration: 100;
        onFinished: {
            tableDelegateContainer.setCellHeight();
        }
    }

    ListModel{
        id: heightModel;
    }

    Rectangle {
        id: selectionBackGround;

        anchors.fill: parent;

        color: tableDelegateContainer.selectedColor;

        radius: 2;
        visible: tableDelegateContainer.selected;
    }

    CheckBox {
        id: checkBox;

        z: 1000;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        checkState: tableDelegateContainer.checkedState;

        visible: tableDelegateContainer.tableItem ? tableDelegateContainer.tableItem.checkable : false;

        onClicked: {
            model.CheckedState = Qt.Checked - model.CheckedState;
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

//        delegate: Loader {
//            sourceComponent: tableDelegateContainer.cellDelegate;
//            onLoaded: {
//                item.pTableDelegateContainer = tableDelegateContainer;
//                item.pDataList = dataList;
//            }
//        }
        delegate: TableCellDelegate {
            pTableDelegateContainer: tableDelegateContainer;
            pDataList: dataList;
        }
    }//dataList

    MouseArea {
        id: ma;

        anchors.fill: parent;

        acceptedButtons: Qt.LeftButton | Qt.RightButton;

        onClicked: {
            if (mouse.button === Qt.RightButton) {
                
                tableDelegateContainer.rightButtonMouseClicked(this.mouseX, this.mouseY);
            }
            tableDelegateContainer.clicked();
        }

        onDoubleClicked: {
            if (mouse.button === Qt.RightButton) {
                return;
            }

            tableDelegateContainer.doubleClicked(this.mouseX, this.mouseY);
        }
    }
}
