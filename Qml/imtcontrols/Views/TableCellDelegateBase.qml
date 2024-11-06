import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Item {
    id: delegateContainer;

    property TableRowDelegateBase rowDelegate: null;

    height: rowDelegate ? rowDelegate.contentHeight : 0;
    width: rowDelegate ? rowDelegate.width/rowDelegate.columnCount : 0;

    property int columnCount: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.columnCount : 0;
    property int rowCount: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.elementsCount : 0;
    property real textLeftIndent: 0
    property real textRightIndent: 0

    property int columnIndex: -1
    property int rowIndex: rowDelegate ? rowDelegate.rowIndex : -1;
    property bool ready: rowCount && rowIndex > -1;

    property string cellHeaderId;

    Component.onDestruction: {
        if (rowDelegate && rowDelegate.tableItem){
            rowDelegate.tableItem.widthRecalc.disconnect(delegateContainer.setCellWidth)
        }
    }

    onReadyChanged:  {
        if(!ready){
            return;
        }
        if (delegateContainer.rowDelegate.horizontalBorderSize){
            if(delegateContainer.rowCount && delegateContainer.rowIndex > 0 && delegateContainer.rowIndex < delegateContainer.rowCount){
                topBorder.createObject(delegateContainer)
            }
            else if(delegateContainer.rowDelegate.visibleTopBorderFirst && delegateContainer.rowIndex == 0 && delegateContainer.rowCount){
                topBorder.createObject(delegateContainer)
            }
        }
        if (delegateContainer.rowDelegate.horizontalBorderSize){
            if(delegateContainer.rowDelegate.visibleBottomBorderLast && delegateContainer.rowIndex == delegateContainer.rowCount -1){
                bottomBorder.createObject(delegateContainer)
            }
        }
    }

    onColumnCountChanged:  {
        if (!delegateContainer.rowDelegate){
            return
        }

        if (delegateContainer.rowDelegate.cellColor !== "transparent"){
            cellBackground.createObject(delegateContainer)
        }

        if (delegateContainer.rowDelegate.verticalBorderSize){
            if(delegateContainer.columnIndex > 0){
                leftBorder.createObject(delegateContainer)
            }
            else if(columnIndex == 0 && delegateContainer.rowDelegate.visibleLeftBorderFirst){
                leftBorder.createObject(delegateContainer)
            }
        }
        if (delegateContainer.rowDelegate.verticalBorderSize){
            if((delegateContainer.columnIndex == delegateContainer.columnCount -1) && delegateContainer.rowDelegate.visibleRightBorderLast){
                rightBorder.createObject(delegateContainer)
            }
        }
        cellHeaderId = delegateContainer.rowDelegate.tableItem.headers.getData("Id", delegateContainer.columnIndex);

        delegateContainer.rowDelegate.tableItem.widthRecalc.connect(delegateContainer.setCellWidth)
    }


    Component{
        id: cellBackground;
        Rectangle{
            anchors.fill: parent;
			z:-4;
            color:  !delegateContainer.rowDelegate ? "transparent" : delegateContainer.rowDelegate.selected ? delegateContainer.rowDelegate.selectedColor: delegateContainer.rowDelegate.cellColor;

        }
    }

    Component{
        id: topBorder;
        Rectangle{
            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.right: parent.right;
            height: !delegateContainer.rowDelegate ? 0 :
                                                     (delegateContainer.rowIndex > 0 && delegateContainer.rowIndex < delegateContainer.rowCount) ? delegateContainer.rowDelegate.horizontalBorderSize :
                                                                                                                                                   (delegateContainer.rowIndex == 0 && delegateContainer.rowDelegate.visibleTopBorderFirst) ? delegateContainer.rowDelegate.horizontalBorderSize : 0;
            color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorHorizontal : "transparent";
        }
    }

    Component{
        id: bottomBorder;
        Rectangle{
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            height: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.visibleBottomBorderLast ? delegateContainer.rowDelegate.horizontalBorderSize : 0 : 0;
            color:  delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorHorizontal : "transparent";

        }
    }

    Component{
        id: leftBorder;
        Rectangle{
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            z: -1;
            width: delegateContainer.rowDelegate ?
                       delegateContainer.rowDelegate.isRightBorder ? delegateContainer.rowDelegate.verticalBorderSize * delegateContainer.rowDelegate.visibleLeftBorderFirst * (delegateContainer.columnIndex == 0)
                                                                   : delegateContainer.rowDelegate.visibleLeftBorderFirst ? delegateContainer.rowDelegate.verticalBorderSize : delegateContainer.columnIndex > 0 ? delegateContainer.rowDelegate.verticalBorderSize : 0
            :0;
            color:  delegateContainer.rowDelegate ?delegateContainer.rowDelegate.borderColorVertical : "transparent";
        }
    }

    Component{
        id: rightBorder;
        Rectangle{
            anchors.right: parent.right;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            z: -2;
            width: delegateContainer.rowDelegate ?
                       !delegateContainer.rowDelegate.isRightBorder ?
                           delegateContainer.rowDelegate.verticalBorderSize * delegateContainer.rowDelegate.visibleRightBorderLast  * (delegateContainer.columnIndex == (delegateContainer.columnCount -1)) * (delegateContainer.columnCount > 0) :
                           delegateContainer.rowDelegate.visibleRightBorderLast ? delegateContainer.rowDelegate.verticalBorderSize  :
                                                                                  delegateContainer.rowDelegate.verticalBorderSize * (delegateContainer.columnIndex < (delegateContainer.columnCount -1)) : 0;

            color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorVertical : "transparent";
        }
    }

    //cornerPatches
    Component{
        id: leftTopCornerPatch;
        Rectangle{
            anchors.left: parent.left;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
                                                                                                              delegateContainer.rowDelegate.cellDecorator.isValidData("LeftTopRound", delegateContainer.columnIndex) ?
                                                                                                                  !delegateContainer.rowDelegate.cellDecorator.getData("LeftTopRound", delegateContainer.columnIndex) :true : 0;
        }
    }

    Component{
        id: rightTopCornerPatch;
        Rectangle{
            anchors.right: parent.right;
            anchors.top: parent.top;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
                                                                                                              delegateContainer.rowDelegate.cellDecorator.isValidData("RightTopRound", delegateContainer.columnIndex) ?
                                                                                                                  !delegateContainer.rowDelegate.cellDecorator.getData("RightTopRound", delegateContainer.columnIndex) :true : 0;
        }
    }

    Component{
        id: leftBottomCornerPatch;
        Rectangle{
            anchors.left: parent.left;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.rowDelegate ?
                         delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
                                                                                  delegateContainer.rowDelegate.cellDecorator.isValidData("LeftBottomRound", delegateContainer.columnIndex) ?
                                                                                      !delegateContainer.rowDelegate.cellDecorator.getData("LeftBottomRound", delegateContainer.columnIndex) :true : 0;
        }
    }

    Component{
        id: rightBottomCornerPatch;
        Rectangle{
            anchors.right:  parent.right;
            anchors.bottom: parent.bottom;
            width: parent.width/2;
            height: parent.height/2;
            color: parent.color;
            visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
                                                                                                              delegateContainer.rowDelegate.cellDecorator.isValidData("RightBottomRound", delegateContainer.columnIndex) ?
                                                                                                                  !delegateContainer.rowDelegate.cellDecorator.getData("RightBottomRound", delegateContainer.columnIndex) :true : 0;
        }
    }

    function getValue(){
        if (delegateContainer && delegateContainer.columnIndex >= 0){
            if(delegateContainer.rowDelegate !== null && delegateContainer.rowDelegate.tableItem !==null && delegateContainer.rowDelegate.dataModel != null){
                let val
                let key = delegateContainer.rowDelegate.tableItem.headers.getData("Id", delegateContainer.columnIndex)
                if ("item" in delegateContainer.rowDelegate.dataModel){
                    val = delegateContainer.rowDelegate.dataModel.item[key]
                }
                else {
                    val = delegateContainer.rowDelegate.dataModel[key];
                }

                return val !== undefined ? val : "";
            }
        }

        return "";
    }

    function setValue(value){
        console.log("setValue", value);
        if (delegateContainer && delegateContainer.columnIndex >= 0){
            if(rowDelegate !== null && rowDelegate.tableItem !==null){
                let tableItem = rowDelegate.tableItem;
                let elements = tableItem.elements;
                let headerId = delegateContainer.rowDelegate.tableItem.headers.getData("Id", delegateContainer.columnIndex);

                elements.setData(headerId, value, delegateContainer.rowIndex);
            }
        }
    }

    function setCellWidth(){
        if(!delegateContainer || !delegateContainer.rowDelegate){
            return;
        }

        var defaultWidth = delegateContainer.columnCount == 0 ? 0 : delegateContainer.rowDelegate.width/delegateContainer.columnCount;
        var widthFromModel = delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.isValidData("Width", delegateContainer.columnIndex) ?
                    delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.getData("Width", delegateContainer.columnIndex) : -1;

        if(!delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.getItemsCount()){
            delegateContainer.width = defaultWidth;
        }
        else if(widthFromModel >= 0){
            delegateContainer.width = widthFromModel;
        }
        else{
            delegateContainer.width = defaultWidth;
        }
    }


}//delegate

