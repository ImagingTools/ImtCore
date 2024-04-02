import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item{
    id: headerDelegate;

    property TableBase table
    property int columnCount: table ? table.columnCount : 0
    property int columnIndex: model.index

    property bool compl: false;
    property bool complCompl: headerDelegate.compl && headerDelegate.table;

    Component.onCompleted: {
        headerDelegate.compl = true;
    }

    Component.onDestruction: {
        headerDelegate.table.widthRecalc.disconnect(headerDelegate.setCellWidth);
    }

    onComplComplChanged: {
        if(headerDelegate.complCompl){
            headerDelegate.table.widthRecalc.connect(headerDelegate.setCellWidth);
            headerDelegate.setCellWidth();
        }
    }

    function setCellWidth(){
        if (!headerDelegate || headerDelegate.columnCount === 0){
            return
        }

        var defaultWidth = (headerDelegate.width)/headerDelegate.columnCount;
        var widthFromModel = headerDelegate.table.widthDecoratorDynamic.IsValidData("Width", headerDelegate.columnIndex) ? headerDelegate.table.widthDecoratorDynamic.GetData("Width", headerDelegate.columnIndex) : -1;

        if(!headerDelegate.table.widthDecoratorDynamic.GetItemsCount()){
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
        height: headerDelegate.table.visibleTopBorderFirst  ? headerDelegate.table.horizontalBorderSize : 0;
        color: headerDelegate.table.borderColorHorizontal;
    }

    Rectangle{
        id: bottomBorder;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: headerDelegate.table.visibleBottomBorderLast ? headerDelegate.table.horizontalBorderSize : 0;
        color:  headerDelegate.table.borderColorHorizontal;
    }

    Rectangle{
        id: leftBorder;
        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: headerDelegate.table.isRightBorder ? headerDelegate.table.verticalBorderSize * headerDelegate.table.visibleLeftBorderFirst * (headerDelegate.columnIndex == 0)
                                            : headerDelegate.table.visibleLeftBorderFirst ? headerDelegate.table.verticalBorderSize : headerDelegate.columnIndex > 0 ? headerDelegate.table.verticalBorderSize : 0;
        color:  headerDelegate.table.borderColorVertical;
    }

    Rectangle{
        id: rightBorder;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: !headerDelegate.table.isRightBorder ?
                   headerDelegate.table.verticalBorderSize * headerDelegate.table.visibleRightBorderLast  * (headerDelegate.columnIndex == (headerDelegate.columnCount -1)) * (headerDelegate.columnCount > 0) :
                   headerDelegate.table.visibleRightBorderLast ? headerDelegate.table.verticalBorderSize  :
                                                           headerDelegate.table.verticalBorderSize * (headerDelegate.columnIndex < (headerDelegate.columnCount -1));

        color: headerDelegate.table.borderColorVertical;
    }


    //borders

    Rectangle{
        id: mainRec;
        anchors.top: topBorder.bottom;
        anchors.left: leftBorder.right;
        anchors.right: rightBorder.left;
        anchors.bottom: bottomBorder.top;
        color: headerDelegate.table.emptyDecorHeader ? "transparent" :
                                                 headerDelegate.table.headerDecorator.IsValidData("Color", headerDelegate.columnIndex) ?
                                                     headerDelegate.table.headerDecorator.GetData("Color", headerDelegate.columnIndex) :
                                                     "transparent";

        opacity:  headerDelegate.table.emptyDecorHeader ? 1 :
                                                    headerDelegate.table.headerDecorator.IsValidData("Opacity", headerDelegate.columnIndex) ?
                                                        headerDelegate.table.headerDecorator.GetData("Opacity", headerDelegate.columnIndex) :
                                                        1;

        radius: headerDelegate.table.emptyDecorHeader ? 0 :
                                                  headerDelegate.table.headerDecorator.IsValidData("CellRadius", headerDelegate.columnIndex) ?
                                                      headerDelegate.table.headerDecorator.GetData("CellRadius", headerDelegate.columnIndex) :0;




        //cornerPatches
        Rectangle{
            id: leftTopCornerPatch;
            anchors.left: parent.left;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: headerDelegate.table.emptyDecorHeader ? true :
                                                       headerDelegate.table.headerDecorator.IsValidData("LeftTopRound", headerDelegate.columnIndex) ?
                                                           !headerDelegate.table.headerDecorator.GetData("LeftTopRound", headerDelegate.columnIndex) :true;
        }

        Rectangle{
            id: rightTopCornerPatch;
            anchors.right: parent.right;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: headerDelegate.table.emptyDecorHeader ? true :
                                                       headerDelegate.table.headerDecorator.IsValidData("RightTopRound", headerDelegate.columnIndex) ?
                                                           !headerDelegate.table.headerDecorator.GetData("RightTopRound", headerDelegate.columnIndex) :true;


        }

        Rectangle{
            id: leftBottomCornerPatch;
            anchors.left: parent.left;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: headerDelegate.table.emptyDecorHeader ? true :
                                                       headerDelegate.table.headerDecorator.IsValidData("LeftBottomRound", headerDelegate.columnIndex) ?
                                                           !headerDelegate.table.headerDecorator.GetData("LeftBottomRound", headerDelegate.columnIndex) :true;
        }

        Rectangle{
            id: rightBottomCornerPatch;
            anchors.right:  parent.right;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: headerDelegate.table.emptyDecorHeader ? true :
                                                       headerDelegate.table.headerDecorator.IsValidData("RightBottomRound", headerDelegate.columnIndex) ?
                                                           !headerDelegate.table.headerDecorator.GetData("RightBottomRound", headerDelegate.columnIndex) :true;
        }
        //cornerPatches



    }//mainRec

    CheckBox {
        id: checkBox;

        z: 1000;

        anchors.verticalCenter: mainRec.verticalCenter;
        anchors.left: mainRec.left;
        anchors.leftMargin: 8;

        checkState: headerDelegate.table.isAllItemChecked ? Qt.Checked : Qt.Unchecked;

        visible: headerDelegate.table.checkable && headerDelegate.columnIndex === 0 && elementsListObj.count > 0 && headerDelegate.table.canSelectAll;

        isActive: !headerDelegate.table.readOnly;

        onClicked: {
            if (headerDelegate.table.readOnly){
                return;
            }

            if (checkBox.checkState === Qt.Checked){
                headerDelegate.table.uncheckAll();
            }
            else{
                headerDelegate.table.checkAll();
            }
        }
    }

    Text {
        id: name;

        anchors.verticalCenter: mainRec.verticalCenter;
        anchors.left: checkBox.visible ? checkBox.right : mainRec.left;
        anchors.right: iconSort.visible ? iconSort.left : mainRec.right;
        anchors.leftMargin: headerDelegate.table.textMarginHor;
        anchors.rightMargin: iconSort.visible ? 0 : headerDelegate.table.textMarginHor;

        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: headerDelegate.table.emptyDecorHeader ? Text.AlignLeft :
                                                               headerDelegate.table.headerDecorator.IsValidData("TextPosition", headerDelegate.columnIndex) ?
                                                                   headerDelegate.table.headerDecorator.GetData("TextPosition", headerDelegate.columnIndex) :
                                                                   Text.AlignLeft;


        font.pixelSize: headerDelegate.table.emptyDecorHeader ? Style.fontSize_common * headerDelegate.scale :
                                                          headerDelegate.table.headerDecorator.IsValidData("FontSize", headerDelegate.columnIndex) ?
                                                              headerDelegate.table.headerDecorator.GetData("FontSize", headerDelegate.columnIndex) :
                                                              Style.fontSize_common * headerDelegate.scale;


        font.family: Style.fontFamilyBold;

        font.bold: headerDelegate.table.emptyDecorHeader ? true :
                                                     headerDelegate.table.headerDecorator.IsValidData("FontBold", headerDelegate.columnIndex) ?
                                                         headerDelegate.table.headerDecorator.GetData("FontBold", headerDelegate.columnIndex) :
                                                         true;


        color: headerDelegate.table.emptyDecorHeader ? Style.textColor :
                                                 headerDelegate.table.headerDecorator.IsValidData("FontColor", headerDelegate.columnIndex) ?
                                                     headerDelegate.table.headerDecorator.GetData("FontColor", headerDelegate.columnIndex) :
                                                     Style.textColor;
        elide: headerDelegate.table.elideMode;

        wrapMode: headerDelegate.table.wrapMode;

        onLinkActivated: {
            Qt.openUrlExternally(link)
        }

        text: model.Name;
    }

    Image {
        id: iconSort;

        anchors.verticalCenter: mainRec.verticalCenter;
        anchors.right: mainRec.right;

        anchors.rightMargin: 5;

        height: 10;
        width: visible ? 10 : 0;

        visible: headerDelegate.table.currentHeaderId === model.Id && headerDelegate.table.hasSort;
        rotation: headerDelegate.table.currentSortOrder == "ASC" ? 180 : 0

        sourceSize.width: width;
        sourceSize.height: height;

        source: headerDelegate.table.sortIndicatorIcon
    }

    ////
    MouseArea {
        id: headerMa;

        anchors.fill: parent;

        visible: headerDelegate.table.hasSort;

        onReleased: {
            headerDelegate.scale = 1;
        }

        onPressed: {
            headerDelegate.scale = 0.985;
        }

        onClicked: {
            headerDelegate.table.headerClicked(model.Id);
        }
    }

    MouseArea{
        id: moving;

        anchors.right:  parent.right;
        anchors.rightMargin: -width/2;

        height: parent.height;
        width: 30;

        visible: headerDelegate.table.canMoveColumns && headerDelegate.columnIndex < headerDelegate.columnCount -1;
        enabled: visible;

        hoverEnabled: true;
        cursorShape: Qt.SplitHCursor;//containsMouse ? Qt.SplitHCursor : containsPress ? Qt.PointingHandCursor : Qt.ArrowCursor;
        //                    cursorShape: containsMouse ? Qt.SplitHCursor : Qt.ArrowCursor;
        property var coord: mapToItem(moving,0,0);
        onPressed: {
            moving.coord = mapToItem(moving,mouse.x,mouse.y)
        }
        onPositionChanged: {
            if(pressed){
                var newCoords = mapToItem(moving,mouse.x,mouse.y);
                var deltaX = Math.trunc(newCoords.x - moving.coord.x);
                var width_ = headerDelegate.table.widthDecoratorDynamic.GetData("Width", headerDelegate.columnIndex);
                var width_next = headerDelegate.table.widthDecoratorDynamic.GetData("Width", headerDelegate.columnIndex+1);
                var width_min = headerDelegate.table.widthDecoratorDynamic.IsValidData("MinWidth", headerDelegate.columnIndex) ? headerDelegate.table.widthDecoratorDynamic.GetData("MinWidth", headerDelegate.columnIndex) : headerDelegate.table.minCellWidth;
                var width_next_min = headerDelegate.table.widthDecoratorDynamic.IsValidData("MinWidth", headerDelegate.columnIndex+1) ? headerDelegate.table.widthDecoratorDynamic.GetData("MinWidth", headerDelegate.columnIndex+1) : headerDelegate.table.minCellWidth;


                width_ += deltaX;
                width_next -= deltaX
                if(width_ > width_min && width_next > width_next_min){
                    headerDelegate.table.widthDecorator.SetData("Width", width_, headerDelegate.columnIndex);
                    headerDelegate.table.widthDecorator.SetData("Width", width_next, headerDelegate.columnIndex+1);

                    headerDelegate.table.setWidth();
                }
            }

        }
        onReleased: {
            headerDelegate.table.saveWidth();
        }
    }

}//delegate

