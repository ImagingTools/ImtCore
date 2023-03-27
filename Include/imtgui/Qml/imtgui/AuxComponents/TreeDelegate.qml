import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: treeDelegate;

    width: 1000;
    height: minHeight;
    color: "transparent";

    visible: model.Visible;

    property int textTopMargin: 8;
    property int count: 0; // bodyArray.length;

    property real minHeight: 40;
    property bool selected: false;
    property bool opened: false;
    property bool hasChildren: model.HasChildren;
    property int depth: model.Depth;

    property int basePadding: 30;

    property var bodyArray:  [];

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
    property int textMarginVer: textTopMargin;

    property bool compl: false;

    signal clicked();
    signal rightButtonMouseClicked(int mX, int mY);
    signal doubleClicked(int mX, int mY);

    signal widthRecalc();

    signal collapsed();
    signal expanded();

    Component.onCompleted: {
        treeDelegate.compl = true;
        treeDelegate.minHeight = treeDelegate.height;
        treeDelegate.setWidth();
    }

    onWidthDecoratorChanged: {
        treeDelegate.setWidth();
    }

    onCellDecoratorChanged: {
        treeDelegate.emptyDecorCell = !treeDelegate.cellDecorator.GetItemsCount();
        treeDelegate.setBorderParams();
    }

    onOpenedChanged: {
        if (treeDelegate.opened){
            treeDelegate.expanded();
        }
        else{
            treeDelegate.collapsed();
        }
    }

    function getItemData(){
        return model;
    }

    function clearArray(){
        while(treeDelegate.bodyArray.length > 0)
            treeDelegate.bodyArray.pop();
        treeDelegate.count = 0;
    }

    function addToArray(str){
        treeDelegate.bodyArray.push(str);
        treeDelegate.count = treeDelegate.bodyArray.length;
    }


    function getSelectedId(){
        return model.Id;
    }

    function getSelectedName(){
        return model.Name;
    }

    function setBorderParams(){
        if(treeDelegate.emptyDecorCell){
            return;
        }
        if(!treeDelegate.canSetBorderParams){
            return;
        }

        if(treeDelegate.cellDecorator.IsValidData("BorderColorHorizontal")){
            treeDelegate.borderColorHorizontal = treeDelegate.cellDecorator.GetData("BorderColorHorizontal");
        }
        if(treeDelegate.cellDecorator.IsValidData("BorderColorVertical")){
            treeDelegate.borderColorVertical = treeDelegate.cellDecorator.GetData("BorderColorVertical");
        }
        if(treeDelegate.cellDecorator.IsValidData("HorizontalBorderSize")){
            treeDelegate.horizontalBorderSize = treeDelegate.cellDecorator.GetData("HorizontalBorderSize");
        }
        if(treeDelegate.cellDecorator.IsValidData("VerticalBorderSize")){
            treeDelegate.verticalBorderSize = treeDelegate.cellDecorator.GetData("VerticalBorderSize");
        }
        if(treeDelegate.cellDecorator.IsValidData("VisibleLeftBorderFirst")){
            treeDelegate.visibleLeftBorderFirst = treeDelegate.cellDecorator.GetData("VisibleLeftBorderFirst");
        }
        if(treeDelegate.cellDecorator.IsValidData("VisibleRightBorderLast")){
            treeDelegate.visibleRightBorderLast = treeDelegate.cellDecorator.GetData("VisibleRightBorderLast");
        }
        if(treeDelegate.cellDecorator.IsValidData("VisibleTopBorderFirst")){
            treeDelegate.visibleTopBorderFirst = treeDelegate.cellDecorator.GetData("VisibleTopBorderFirst");
        }
        if(treeDelegate.cellDecorator.IsValidData("VisibleBottomBorderLast")){
            treeDelegate.visibleBottomBorderLast = treeDelegate.cellDecorator.GetData("VisibleBottomBorderLast");
        }
        if(treeDelegate.cellDecorator.IsValidData("WrapMode")){
            treeDelegate.wrapMode = treeDelegate.cellDecorator.GetData("WrapMode");
        }
        if(treeDelegate.cellDecorator.IsValidData("IsRightBorder")){
            treeDelegate.isRightBorder = treeDelegate.cellDecorator.GetData("IsRightBorder");
        }
        if(treeDelegate.cellDecorator.IsValidData("ElideMode")){
            treeDelegate.elideMode = treeDelegate.cellDecorator.GetData("ElideMode");
        }

    }

    function setWidth(){

        treeDelegate.widthDecoratorDynamic.Clear();
        treeDelegate.widthDecoratorDynamic.Copy(treeDelegate.widthDecorator);

        if(!treeDelegate.widthDecorator.GetItemsCount()){
            treeDelegate.widthRecalc();
            return;
        }

        var count_ = 0;
        var lengthMinus = 0;

        for(var i = 0; i < treeDelegate.widthDecorator.GetItemsCount(); i++){

            var width_ = treeDelegate.widthDecorator.IsValidData("Width",i) ? treeDelegate.widthDecorator.GetData("Width",i): -1;
            var widthPercent_ = treeDelegate.widthDecorator.IsValidData("WidthPercent",i) ? treeDelegate.widthDecorator.GetData("WidthPercent",i): -1;

            if((width_ == -1) && (widthPercent_ == -1)){
                count_++;
            }
            else{
                width_ = width_< 0 ? 0 : width_;
                widthPercent_ = widthPercent_ < 0 ? 0 : widthPercent_*treeDelegate.width/100;
                lengthMinus += Math.max(width_,widthPercent_);
            }
        }

        if((treeDelegate.width - lengthMinus) < 0 || count_ == treeDelegate.widthDecorator.GetItemsCount() ){
            treeDelegate.widthDecoratorDynamic.Clear();
            treeDelegate.widthRecalc();
            return;
        }

        for(var i = 0; i < treeDelegate.widthDecorator.GetItemsCount(); i++){

            var width_ = treeDelegate.widthDecorator.IsValidData("Width",i) ? treeDelegate.widthDecorator.GetData("Width",i): -1;
            var widthPercent_ = treeDelegate.widthDecorator.IsValidData("WidthPercent",i) ? treeDelegate.widthDecorator.GetData("WidthPercent",i): -1;

            if(width_ < 0  && widthPercent_ < 0 ){
                if(count_){
                    treeDelegate.widthDecoratorDynamic.SetData("Width",(treeDelegate.width - lengthMinus)/count_,i);
                }
            }
            else if(width_ < 0  && widthPercent_ >= 0){
                treeDelegate.widthDecoratorDynamic.SetData("Width", widthPercent_*treeDelegate.width/100,i);
            }

            width_ = treeDelegate.widthDecoratorDynamic.IsValidData("Width",i) ? treeDelegate.widthDecoratorDynamic.GetData("Width",i): -1;

            if(width_ < 0){
                if(count_){
                    treeDelegate.widthDecoratorDynamic.SetData("Width",(treeDelegate.width - lengthMinus)/count_,i);
                }

            }

        }

        treeDelegate.widthRecalc();
    }

    onWidthChanged: {
        treeDelegate.setWidth();
        if(treeDelegate.wrapMode !== Text.NoWrap){
            pause.stop();
            pause.start();
        }

    }

    onCountChanged: {
        if(treeDelegate.wrapMode !== Text.NoWrap){
            heightModel.append({"cellHeight": 0});
            pause.stop();
            pause.start();
        }
    }

    function setCellHeight(){
        if(treeDelegate.wrapMode == Text.NoWrap){
            return;
        }

        var maxVal = 0;
        for(var i = 0; i < heightModel.count; i++){
            var currVal = heightModel.get(i).cellHeight;
            if(currVal > maxVal){
                maxVal = currVal;
            }
        }
        treeDelegate.height = Math.max(maxVal, treeDelegate.minHeight);
    }


    PauseAnimation {
        id: pause;
        duration: 100;
        onFinished: {
            treeDelegate.setCellHeight();
        }
    }

    ListModel{
        id: heightModel;
    }

    Rectangle {
        id: selectionBackGround;

        anchors.fill: parent;

        color: treeDelegate.selectedColor;

        radius: 2;
        visible: treeDelegate.selected;
    }

    AuxButton {
        id: arrowButton;

        z: 1000;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        width: 15;
        height: width;

        visible: treeDelegate.hasChildren;

        iconSource: treeDelegate.opened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                        "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

        onClicked: {
            treeDelegate.opened = !treeDelegate.opened;
        }
    }

    ListView {
        id: dataList;
        anchors.fill: parent;
        anchors.left: arrowButton.right;
        anchors.leftMargin: 10;
        clip: true;
        orientation: ListView.Horizontal;
        spacing: 0;
        property bool compl: false;
        Component.onCompleted: {
            dataList.compl = true;
        }
        model: treeDelegate.count;

        delegate: Item {
            id: deleg;
            height: treeDelegate.height;
            width: treeDelegate.width/treeDelegate.count;

            property bool compl: false;
            property bool complCompl: deleg.compl && dataList.compl;

            Component.onCompleted: {
                deleg.compl = true;
            }

            onComplComplChanged: {
                if(deleg.complCompl){
                    treeDelegate.widthRecalc.connect(deleg.setCellWidth)
                    deleg.setCellWidth();
                }

            }

            function setCellWidth(){
                if(!deleg.complCompl){
                    return;
                }

                var defaultWidth = dataList.count == 0 ? 0 : treeDelegate.width/dataList.count;
                var widthFromModel = treeDelegate.widthDecoratorDynamic.IsValidData("Width", model.index) ? treeDelegate.widthDecoratorDynamic.GetData("Width", model.index) : -1;

                if(!treeDelegate.widthDecoratorDynamic.GetItemsCount()){
                    deleg.width = defaultWidth;
                }
                else if(widthFromModel >= 0){
                    deleg.width = widthFromModel;
                }
                else{
                    deleg.width = defaultWidth;
                }

            }

            //borders

            Rectangle{
                id: topBorder;
                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.right: parent.right;
                height: treeDelegate.visibleTopBorderFirst  ? treeDelegate.horizontalBorderSize : 0;
                color: treeDelegate.borderColorHorizontal;
            }

            Rectangle{
                id: bottomBorder;
                anchors.bottom: parent.bottom;
                anchors.left: parent.left;
                anchors.right: parent.right;
                height: treeDelegate.visibleBottomBorderLast ? treeDelegate.horizontalBorderSize : 0;
                color:  treeDelegate.borderColorHorizontal;
            }

            Rectangle{
                id: leftBorder;
                anchors.left: parent.left;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                width: treeDelegate.isRightBorder ? treeDelegate.verticalBorderSize * treeDelegate.visibleLeftBorderFirst * (model.index == 0)
                                                            : treeDelegate.visibleLeftBorderFirst ? treeDelegate.verticalBorderSize : model.index > 0 ? treeDelegate.verticalBorderSize : 0;
                color:  treeDelegate.borderColorVertical;
            }

            Rectangle{
                id: rightBorder;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                width: !treeDelegate.isRightBorder ?
                           treeDelegate.verticalBorderSize * treeDelegate.visibleRightBorderLast  * (model.index == (treeDelegate.count -1)) * (treeDelegate.count > 0) :
                           treeDelegate.visibleRightBorderLast ? treeDelegate.verticalBorderSize  :
                                                                           treeDelegate.verticalBorderSize * (model.index < (treeDelegate.count -1));

                color: treeDelegate.borderColorVertical;
            }


            //borders

            Rectangle{
                id: mainRec;
                anchors.top: topBorder.bottom;
                anchors.left: leftBorder.right;
                anchors.right: rightBorder.left;
                anchors.bottom: bottomBorder.top;
                color: treeDelegate.emptyDecorCell ? "transparent" :
                                                               treeDelegate.cellDecorator.IsValidData("Color", model.index) ?
                                                                   treeDelegate.cellDecorator.GetData("Color", model.index) :
                                                                   "transparent";


                radius: treeDelegate.emptyDecorCell ? 0 :
                                                                treeDelegate.cellDecorator.IsValidData("CellRadius", model.index) ?
                                                                    treeDelegate.cellDecorator.GetData("CellRadius", model.index) :0;




                //cornerPatches
                Rectangle{
                    id: leftTopCornerPatch;
                    anchors.left: parent.left;
                    anchors.top: parent.top;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: treeDelegate.emptyDecorCell ? true :
                                                                     treeDelegate.cellDecorator.IsValidData("LeftTopRound", model.index) ?
                                                                         !treeDelegate.cellDecorator.GetData("LeftTopRound", model.index) :true;
                }

                Rectangle{
                    id: rightTopCornerPatch;
                    anchors.right: parent.right;
                    anchors.top: parent.top;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: treeDelegate.emptyDecorCell ? true :
                                                                     treeDelegate.cellDecorator.IsValidData("RightTopRound", model.index) ?
                                                                         !treeDelegate.cellDecorator.GetData("RightTopRound", model.index) :true;


                }

                Rectangle{
                    id: leftBottomCornerPatch;
                    anchors.left: parent.left;
                    anchors.bottom: parent.bottom;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: treeDelegate.emptyDecorCell ? true :
                                                                     treeDelegate.cellDecorator.IsValidData("LeftBottomRound", model.index) ?
                                                                         !treeDelegate.cellDecorator.GetData("LeftBottomRound", model.index) :true;


                }

                Rectangle{
                    id: rightBottomCornerPatch;
                    anchors.right:  parent.right;
                    anchors.bottom: parent.bottom;
                    width: parent.width/2;
                    height: parent.height/2;
                    color: parent.color;
                    visible: treeDelegate.emptyDecorCell ? true :
                                                                     treeDelegate.cellDecorator.IsValidData("RightBottomRound", model.index) ?
                                                                         !treeDelegate.cellDecorator.GetData("RightBottomRound", model.index) :true;

                }

                //cornerPatches




            }//mainRec

            Rectangle {
                id: selectionRec;

                anchors.left: parent.left;
                anchors.right: parent.right;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                anchors.topMargin: topBorder.height;
                anchors.bottomMargin: bottomBorder.height;

                color: treeDelegate.selectedColor;
                opacity: treeDelegate.selectedOpacity;

                radius: mainRec.radius;
                visible: treeDelegate.selected && mainRec.color !== "transparent";
            }

            Text {
                id: name;

                anchors.verticalCenter: mainRec.verticalCenter;
                anchors.left: mainRec.left;
                anchors.right: mainRec.right;
                anchors.leftMargin: treeDelegate.textMarginHor;
                anchors.rightMargin: treeDelegate.textMarginHor;


                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: treeDelegate.emptyDecorCell ? Text.AlignLeft :
                                                                             treeDelegate.cellDecorator.IsValidData("TextPosition", model.index) ?
                                                                                 treeDelegate.cellDecorator.GetData("TextPosition", model.index) :
                                                                                 Text.AlignLeft;




                font.pixelSize: treeDelegate.emptyDecorCell ? Style.fontSize_common:
                                                                        treeDelegate.cellDecorator.IsValidData("FontSize", model.index) ?
                                                                            treeDelegate.cellDecorator.GetData("FontSize", model.index) :
                                                                            Style.fontSize_common;


                font.family: Style.fontFamily;

                font.bold: treeDelegate.emptyDecorCell ? true :
                                                                   treeDelegate.cellDecorator.IsValidData("FontBold", model.index) ?
                                                                       treeDelegate.cellDecorator.GetData("FontBold", model.index) :
                                                                       true;


                color: treeDelegate.emptyDecorCell ? Style.textColor :
                                                               treeDelegate.cellDecorator.IsValidData("FontColor", model.index) ?
                                                                   treeDelegate.cellDecorator.GetData("FontColor", model.index) :
                                                                   Style.textColor;

                elide: treeDelegate.elideMode;

                wrapMode: treeDelegate.wrapMode;

                onLinkActivated: {
                    Qt.openUrlExternally(link);
                }

                text: treeDelegate.bodyArray[model.index] == undefined ? "" :
                                                                                  treeDelegate.bodyArray[model.index];


                onHeightChanged: {
                    if(treeDelegate.wrapMode !== Text.NoWrap){
                        if(model.index < heightModel.count){
                            var height_ = name.height +
                                    2*treeDelegate.textMarginVer +
                                    topBorder.height + bottomBorder.height;

                            heightModel.setProperty(model.index, "cellHeight", height_);

                        }
                    }
                }


            }

        }//delegate

    }//dataList


    MouseArea {
        id: ma;

        anchors.fill: parent;

        acceptedButtons: Qt.LeftButton | Qt.RightButton;

        onClicked: {
            if (mouse.button === Qt.RightButton) {
                console.log("TableDelegate onRightButtonMouseClicked");
                treeDelegate.rightButtonMouseClicked(this.mouseX, this.mouseY);
            }
            treeDelegate.clicked();
        }

        onDoubleClicked: {
            if (mouse.button === Qt.RightButton) {
                return;
            }

            treeDelegate.doubleClicked(this.mouseX, this.mouseY);
        }
    }
}
