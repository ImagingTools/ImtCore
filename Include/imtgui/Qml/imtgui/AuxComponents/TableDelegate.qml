import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: tableDelegateContainer;

    width: 1000;
    height: minHeight;
    color: "transparent";

    property int textTopMargin: 8;
    property int count: 0; // bodyArray.length;

    property real minHeight: 40;
    property bool selected: false;

    property var bodyArray:  [];


    property TreeItemModel cellDecorator : TreeItemModel{};
    property TreeItemModel widthDecorator : TreeItemModel{};
    property TreeItemModel widthDecoratorDynamic : TreeItemModel{};

    property bool emptyDecorCell: true;

    property string selectedColor: Style.selectedColor;

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
    property int textMarginVer: textTopMargin;


    signal clicked();
    signal rightButtonMouseClicked(int mX, int mY);
    signal doubleClicked(int mX, int mY);

    signal widthRecalc();

    Component.onCompleted: {
        tableDelegateContainer.minHeight = tableDelegateContainer.height;
        tableDelegateContainer.setWidth();
    }

    onWidthDecoratorChanged: {
        tableDelegateContainer.setWidth();
    }

    onCellDecoratorChanged: {
        tableDelegateContainer.emptyDecorCell = !tableDelegateContainer.cellDecorator.GetItemsCount();
        tableDelegateContainer.setBorderParams();
    }


    function getItemData(){
        return model;
    }

    function clearArray(){
        while(tableDelegateContainer.bodyArray.length > 0)
            tableDelegateContainer.bodyArray.pop();
        tableDelegateContainer.count = 0;
    }

    function addToArray(str){
        tableDelegateContainer.bodyArray.push(str);
        tableDelegateContainer.count = tableDelegateContainer.bodyArray.length;
    }


    function getSelectedId(){
        return model.Id;
    }

    function getSelectedName(){
        return model.Name;
    }

    function setBorderParams(){
        if(tableDelegateContainer.emptyDecorCell || !tableDelegateContainer.canSetBorderParams){
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
        tableDelegateContainer.height = Math.max(maxVal, minHeight);
    }


    PauseAnimation {
        id: pause;
        duration: 100;
        onFinished: {
            setCellHeight();
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
        visible: selected;
    }


    ListView {
        id: dataList;

        anchors.fill: parent;

        clip: true;
        orientation: ListView.Horizontal;
        spacing: 0;
        model: tableDelegateContainer.count;

        delegate: Item {
            id: deleg;


            height: tableDelegateContainer.height;

            Connections{
                target: tableDelegateContainer;
                function onWidthRecalc(){
                    deleg.setCellWidth()
                }
            }

            Component.onCompleted: {
                //tableDelegateContainer.widthRecalc.connect(deleg.setCellWidth)
                deleg.setCellWidth();
            }

            function setCellWidth(){

                var defaultWidth = dataList.count == 0 ? 0 : tableDelegateContainer.width/dataList.count;
                var widthFromModel = widthDecoratorDynamic.IsValidData("Width", model.index) ? widthDecoratorDynamic.GetData("Width", model.index) : -1;

                if(!tableDelegateContainer.widthDecoratorDynamic.GetItemsCount()){
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
                id: leftBorder;
                anchors.left: parent.left;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                width: tableDelegateContainer.isRightBorder ? tableDelegateContainer.verticalBorderSize * tableDelegateContainer.visibleLeftBorderFirst * (model.index == 0)
                                                            : tableDelegateContainer.visibleLeftBorderFirst ? tableDelegateContainer.verticalBorderSize : model.index > 0 ? tableDelegateContainer.verticalBorderSize : 0;
                color:  tableDelegateContainer.borderColorVertical;
            }

            Rectangle{
                id: rightBorder;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                width: !tableDelegateContainer.isRightBorder ?
                           tableDelegateContainer.verticalBorderSize * tableDelegateContainer.visibleRightBorderLast  * (model.index == (tableDelegateContainer.count -1)) * (tableDelegateContainer.count > 0) :
                           tableDelegateContainer.visibleRightBorderLast ? tableDelegateContainer.verticalBorderSize  :
                                                                           tableDelegateContainer.verticalBorderSize * (model.index < (tableDelegateContainer.count -1));

                color: tableDelegateContainer.borderColorVertical;
            }

            Rectangle{
                id: topBorder;
                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.right: parent.right;
                height: tableDelegateContainer.visibleTopBorderFirst  ? tableDelegateContainer.horizontalBorderSize : 0;
                color: tableDelegateContainer.borderColorHorizontal;
            }

            Rectangle{
                id: bottomBorder;
                anchors.bottom: parent.bottom;
                anchors.left: parent.left;
                anchors.right: parent.right;
                height: tableDelegateContainer.visibleBottomBorderLast ? tableDelegateContainer.horizontalBorderSize : 0;
                color:  tableDelegateContainer.borderColorHorizontal;
            }
            //borders

            Rectangle{
                id: mainRec;
                anchors.top: topBorder.height > 0 ? topBorder.bottom : parent.top;
                anchors.left: leftBorder.width > 0 ? leftBorder.right : parent.left;
                anchors.right: rightBorder.width > 0 ? rightBorder.left : parent.right;
                anchors.bottom: bottomBorder.height > 0 ? bottomBorder.top : parent.bottom;
                color: tableDelegateContainer.emptyDecorCell ? "transparent" :
                                                               cellDecorator.IsValidData("Color", model.index) ?
                                                                   cellDecorator.GetData("Color", model.index) :
                                                                   "transparent";


                radius: tableDelegateContainer.emptyDecorCell ? 0 :
                                                                cellDecorator.IsValidData("CellRadius", model.index) ?
                                                                    cellDecorator.GetData("CellRadius", model.index) :0;




                //cornerPatches
                Rectangle{
                    id: leftTopCornerPatch;
                    anchors.left: parent.left;
                    anchors.top: parent.top;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: tableDelegateContainer.emptyDecorCell ? true :
                                                                     cellDecorator.IsValidData("LeftTopRound", model.index) ?
                                                                         !cellDecorator.GetData("LeftTopRound", model.index) :true;
                }

                Rectangle{
                    id: rightTopCornerPatch;
                    anchors.right: parent.right;
                    anchors.top: parent.top;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: tableDelegateContainer.emptyDecorCell ? true :
                                                                     cellDecorator.IsValidData("RightTopRound", model.index) ?
                                                                         !cellDecorator.GetData("RightTopRound", model.index) :true;


                }

                Rectangle{
                    id: leftBottomCornerPatch;
                    anchors.left: parent.left;
                    anchors.bottom: parent.bottom;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: tableDelegateContainer.emptyDecorCell ? true :
                                                                     cellDecorator.IsValidData("LeftBottomRound", model.index) ?
                                                                         !cellDecorator.GetData("LeftBottomRound", model.index) :true;


                }

                Rectangle{
                    id: rightBottomCornerPatch;
                    anchors.right:  parent.right;
                    anchors.bottom: parent.bottom;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: tableDelegateContainer.emptyDecorCell ? true :
                                                                     cellDecorator.IsValidData("RightBottomRound", model.index) ?
                                                                         !cellDecorator.GetData("RightBottomRound", model.index) :true;

                }

                //cornerPatches

                Text {
                    id: name;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.right: parent.right;
                    anchors.leftMargin: tableDelegateContainer.textMarginHor;
                    anchors.rightMargin: tableDelegateContainer.textMarginHor;


                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: tableDelegateContainer.emptyDecorCell ? Text.AlignLeft :
                                                                                 cellDecorator.IsValidData("TextPosition", model.index) ?
                                                                                     cellDecorator.GetData("TextPosition", model.index) :
                                                                                     Text.AlignLeft;




                    font.pixelSize: tableDelegateContainer.emptyDecorCell ? Style.fontSize_common:
                                                                            cellDecorator.IsValidData("FontSize", model.index) ?
                                                                                cellDecorator.GetData("FontSize", model.index) :
                                                                                Style.fontSize_common;


                    font.family: Style.fontFamily;

                    font.bold: tableDelegateContainer.emptyDecorCell ? true :
                                                                       cellDecorator.IsValidData("FontBold", model.index) ?
                                                                           cellDecorator.GetData("FontBold", model.index) :
                                                                           true;


                    color: tableDelegateContainer.emptyDecorCell ? Style.textColor :
                                                                   cellDecorator.IsValidData("FontColor", model.index) ?
                                                                       cellDecorator.GetData("FontColor", model.index) :
                                                                       Style.textColor;

                    elide: tableDelegateContainer.elideMode;

                    wrapMode: tableDelegateContainer.wrapMode;

                    onLinkActivated: {
                        Qt.openUrlExternally(link);
                    }

                    text: tableDelegateContainer.bodyArray[model.index] == undefined ? "" :
                                                                                      tableDelegateContainer.bodyArray[model.index];


                    onHeightChanged: {
                        if(tableDelegateContainer.wrapMode !== Text.NoWrap){
                            if(model.index < heightModel.count){
                                var height_ = name.height +
                                        2*tableDelegateContainer.textMarginVer +
                                        topBorder.height + bottomBorder.height;

                                heightModel.setProperty(model.index, "cellHeight", height_);

                            }
                        }
                    }


                }


            }//mainRec



        }//delegate

    }//dataList


    MouseArea {
        id: ma;

        anchors.fill: parent;

        acceptedButtons: Qt.LeftButton | Qt.RightButton;

        onClicked: {
            if (mouse.button === Qt.RightButton) {
                console.log("TableDelegate onRightButtonMouseClicked");
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
