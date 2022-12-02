import QtQuick 2.12
import Acf 1.0

Item {
    id: tableContainer;

    property int selectedIndex: -1;
    property int itemHeight: 35;
    property int headerHeight: 35;
    property int headerMinHeight: 35;

    property bool hasFilter: false;
    property bool hasSort: false;

    property bool showHeaders: true;

    property TreeItemModel headers; //: elementsList.model;

    property TreeItemModel tableDecorator : TreeItemModel{};

    property TreeItemModel headerDecorator : TreeItemModel{};
    property TreeItemModel cellDecorator : TreeItemModel{};
    property TreeItemModel widthDecorator : TreeItemModel{};
    property TreeItemModel widthDecoratorDynamic : TreeItemModel{};

    property alias delegate: elementsList.delegate;
    property alias elements: elementsList.model;
    property alias elementsList: elementsList;

    property alias headerDelegate: headersList.delegate;
    property real headerElementWidth: (headersList.width)/headersList.count;
    property alias headerElementHeight: headersList.height;

    property alias backgroundElementsColor: elementsBg.color;

    property int radius: 7;

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
    //

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
    }


    onTableDecoratorChanged: {

        tableContainer.headerDecorator = tableContainer.tableDecorator.GetTreeItemModel("Headers");
        tableContainer.cellDecorator = tableContainer.tableDecorator.GetTreeItemModel("Cells");
        tableContainer.widthDecorator = tableContainer.tableDecorator.GetTreeItemModel("CellWidth");

        tableContainer.emptyDecor = !tableDecorator.GetItemsCount();
        tableContainer.emptyDecorHeader = !headerDecorator.GetItemsCount();

        tableContainer.setBorderParams();
        tableContainer.setWidth();

        if(tableContainer.wrapMode !== Text.NoWrap){
            for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
                heightModel.append({"cellHeight": 0});
            }
            tableContainer.heightRecalc();
            pause.stop();
            pause.start();
        }


    }

    onSelectedIndexChanged: {
        if (selectedIndex > -1){
            elementsList.forceActiveFocus();
        }
    }

    onWidthChanged: {
        tableContainer.setWidth();
        if(tableContainer.wrapMode !== Text.NoWrap){
            pause.stop();
            pause.start();
        }
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


    function setWidth(){

        tableContainer.widthDecoratorDynamic.Clear();
        tableContainer.widthDecoratorDynamic.Copy(tableContainer.widthDecorator);

        if(!tableContainer.tableDecorator.GetItemsCount()||!tableContainer.widthDecorator.GetItemsCount()){
            tableContainer.widthRecalc();
            return;
        }

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

        if((tableContainer.width - lengthMinus) < 0 || count_ == tableContainer.widthDecorator.GetItemsCount() ){
            tableContainer.widthDecoratorDynamic.Clear();
            tableContainer.widthRecalc();
            return;
        }

        for(var i = 0; i < tableContainer.widthDecorator.GetItemsCount(); i++){

            var width_ = tableContainer.widthDecorator.IsValidData("Width",i) ? tableContainer.widthDecorator.GetData("Width",i): -1;
            var widthPercent_ = tableContainer.widthDecorator.IsValidData("WidthPercent",i) ? tableContainer.widthDecorator.GetData("WidthPercent",i): -1;

            if(width_ < 0  && widthPercent_ < 0 ){
                if(count_){
                    tableContainer.widthDecoratorDynamic.SetData("Width",(tableContainer.width - lengthMinus)/count_,i);
                }
            }
            else if(width_ < 0  && widthPercent_ >= 0){
                tableContainer.widthDecoratorDynamic.SetData("Width", widthPercent_*tableContainer.width/100,i);
            }

            width_ = tableContainer.widthDecoratorDynamic.IsValidData("Width",i) ? tableContainer.widthDecoratorDynamic.GetData("Width",i): -1;

            if(width_ < 0){
                if(count_){
                    tableContainer.widthDecoratorDynamic.SetData("Width",(tableContainer.width - lengthMinus)/count_,i);
                }

            }

        }

        tableContainer.widthRecalc();
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


    function getSelectedId(){
        console.log("getSelectedId");
        if (tableContainer.selectedIndex > -1){
            let item = elementsList.itemAtIndex(tableContainer.selectedIndex);
            return item.getSelectedId();
        }

        return "";
    }

    function getSelectedName(){
        if (tableContainer.selectedIndex > -1){
            let item = elementsList.itemAtIndex(tableContainer.selectedIndex);
            return item.getSelectedName();
        }

        return "";
    }

    function getSelectedItemData(){
        if (tableContainer.selectedIndex > -1){
            let item = elementsList.itemAtIndex(tableContainer.selectedIndex);
            return item.getItemData();
        }

        return null;
    }

    MouseArea {
        id: maTable;

        anchors.fill: parent;

        onClicked: {
            console.log("AuxTable MouseArea onClicked");
            tableContainer.selectedIndex = -1;
        }
    }

    Item {
        id: headersPanel;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: visible ? tableContainer.headerHeight: 0;

        visible: headersList.count > 0 && tableContainer.showHeaders;

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
                    if(!deleg.complCompl){
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
                    anchors.top: topBorder.height > 0 ? topBorder.bottom : parent.top;
                    anchors.left: leftBorder.width > 0 ? leftBorder.right : parent.left;
                    anchors.right: rightBorder.width > 0 ? rightBorder.left : parent.right;
                    anchors.bottom: bottomBorder.height > 0 ? bottomBorder.top : parent.bottom;
                    color: tableContainer.emptyDecorHeader ? Style.baseColor :
                                                             headerDecorator.IsValidData("Color", model.index) ?
                                                                 headerDecorator.GetData("Color", model.index) :
                                                                 Style.baseColor;


                    radius: tableContainer.emptyDecorHeader ? 0 :
                                                              headerDecorator.IsValidData("CellRadius", model.index) ?
                                                                  headerDecorator.GetData("CellRadius", model.index) :0;




                    //cornerPatches
                    Rectangle{
                        id: leftTopCornerPatch;
                        anchors.left: parent.left;
                        anchors.top: parent.top;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   headerDecorator.IsValidData("LeftTopRound", model.index) ?
                                                                       !headerDecorator.GetData("LeftTopRound", model.index) :true;
                    }

                    Rectangle{
                        id: rightTopCornerPatch;
                        anchors.right: parent.right;
                        anchors.top: parent.top;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   headerDecorator.IsValidData("RightTopRound", model.index) ?
                                                                       !headerDecorator.GetData("RightTopRound", model.index) :true;


                    }

                    Rectangle{
                        id: leftBottomCornerPatch;
                        anchors.left: parent.left;
                        anchors.bottom: parent.bottom;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   headerDecorator.IsValidData("LeftBottomRound", model.index) ?
                                                                       !headerDecorator.GetData("LeftBottomRound", model.index) :true;


                    }

                    Rectangle{
                        id: rightBottomCornerPatch;
                        anchors.right:  parent.right;
                        anchors.bottom: parent.bottom;
                        width: parent.width/2;
                        height: parent.height/2;
                        color: parent.color;
                        visible: tableContainer.emptyDecorHeader ? true :
                                                                   headerDecorator.IsValidData("RightBottomRound", model.index) ?
                                                                       !headerDecorator.GetData("RightBottomRound", model.index) :true;

                    }
                    //cornerPatches

                    Text {
                        id: name;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.right: iconSort.visible ? iconSort.left : parent.right;
                        anchors.leftMargin: tableContainer.textMarginHor;
                        anchors.rightMargin: iconSort.visible ? 0 : tableContainer.textMarginHor;

                        verticalAlignment: Text.AlignVCenter;
                        horizontalAlignment: tableContainer.emptyDecorHeader ? Text.AlignLeft :
                                                                               headerDecorator.IsValidData("TextPosition", model.index) ?
                                                                                   headerDecorator.GetData("TextPosition", model.index) :
                                                                                   Text.AlignLeft;




                        font.pixelSize: tableContainer.emptyDecorHeader ? Style.fontSize_common * deleg.scale :
                                                                          headerDecorator.IsValidData("FontSize", model.index) ?
                                                                              headerDecorator.GetData("FontSize", model.index) :
                                                                              Style.fontSize_common * deleg.scale;


                        font.family: Style.fontFamilyBold;

                        font.bold: tableContainer.emptyDecorHeader ? true :
                                                                     headerDecorator.IsValidData("FontBold", model.index) ?
                                                                         headerDecorator.GetData("FontBold", model.index) :
                                                                         true;


                        color: tableContainer.emptyDecorHeader ? Style.textColor :
                                                                 headerDecorator.IsValidData("FontColor", model.index) ?
                                                                     headerDecorator.GetData("FontColor", model.index) :
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

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.right: parent.right;
                        anchors.rightMargin: visible ? Math.max(tableContainer.textMarginHor, tableContainer.verticalBorderSize) : 0;

                        height: 10;
                        width: visible ? 10 : 0;

                        visible: headersList.currentIndex === model.index && tableContainer.hasSort;

                        sourceSize.width: width;
                        sourceSize.height: height;

                        onStateChanged: {
                            console.log("onStateChanged", state);
                        }

                        state: "up";

                        states: [
                            State {
                                name: "up"
                                PropertyChanges {
                                    target: iconSort;
                                    source: "../../../Icons/" + Style.theme + "/Up_On_Normal.svg";
                                }
                            },
                            State {
                                name: "down"
                                PropertyChanges {
                                    target: iconSort;
                                    source: "../../../Icons/" + Style.theme + "/Down_On_Normal.svg";
                                }
                            }
                        ]
                    }

                }//mainRec


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
                        headersList.currentIndex = model.index;

                        if (iconSort.state == "up"){
                            iconSort.state = "down";
                        }
                        else{
                            iconSort.state = "up";
                        }

                        tableContainer.headerClicked(model.Id);
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

    AuxButton {
        id: iconFilter;

        anchors.verticalCenter: headersPanel.verticalCenter;
        anchors.right: headersPanel.right;
        anchors.rightMargin: -width;

        visible: tableContainer.hasFilter && tableContainer.showHeaders;

        width: tableContainer.hasFilter ? 20 : 0;
        height: width;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Filter_On_Normal.svg";

        onClicked: {
            console.log("AuxButton iconFilter onClicked");
            filterClicked();
        }
    }

    Rectangle {
        id: elementsBg;

        anchors.fill: elementsList;
        color: Style.baseColor;
        radius: tableContainer.radius;
    }



    ListView {
        id: elementsList;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: headersPanel.bottom;
        anchors.bottom: parent.bottom;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;
        Keys.onUpPressed: {
            if (tableContainer.selectedIndex > 0){
                tableContainer.selectedIndex--;
            }
            else{
                tableContainer.selectedIndex = elementsList.count - 1;
            }
        }

        Keys.onDownPressed: {
            if (tableContainer.selectedIndex < elementsList.count - 1){
                tableContainer.selectedIndex++;
            }
            else{
                tableContainer.selectedIndex = 0;
            }
        }


        delegate:

            TableDelegate {

            id: tableDelegate;

            height: tableContainer.itemHeight;
            width: elementsList.width;
            minHeight: tableContainer.itemHeight;

            selected: tableContainer.selectedIndex === model.index;

            //!!!
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
                for(var i = 0; i < tableContainer.headers.GetItemsCount(); i++){
                    tableDelegate.addToArray(model[tableContainer.headers.GetData("Id", i)]);
                }
            }

            onClicked: {
                tableContainer.selectedIndex = model.index;

            }

            onRightButtonMouseClicked: {
                console.log("AuxTable onRightButtonMouseClicked", mX, mY);
                var point = mapToItem(thumbnailDecoratorContainer, mX, mY);
                tableContainer.rightButtonMouseClicked(point.x, point.y);
            }

            onDoubleClicked: {
                console.log("onDoubleClicked", model["Id"], model["Name"])
                var point = mapToItem(thumbnailDecoratorContainer, mX, mY);
                tableContainer.doubleClicked(point.x, point.y)
                tableContainer.selectItem(model.Id, model.Name);

            }
        }

    }//Elements ListView
}
