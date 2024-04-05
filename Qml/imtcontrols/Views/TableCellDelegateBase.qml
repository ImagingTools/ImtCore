import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Item {
    id: delegateContainer;

    property TableRowDelegateBase rowDelegate: null;

    height: rowDelegate ? rowDelegate.contentHeight : 0;
    width: rowDelegate ? rowDelegate.width/rowDelegate.columnCount : 0;

    property int columnCount: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.columnCount : 0;

    property real textLeftIndent: 0
    property real textRightIndent: 0

    property int columnIndex: -1
    property int rowIndex: rowDelegate ? rowDelegate.rowIndex : -1;

    Component.onDestruction: {
        if (rowDelegate && rowDelegate.tableItem){
            rowDelegate.tableItem.widthRecalc.disconnect(delegateContainer.setCellWidth)
        }
    }


    onRowDelegateChanged: {
        if (!delegateContainer.rowDelegate){
            return
        }
        if (delegateContainer.rowDelegate.visibleTopBorderFirst){
            topBorder.createObject(delegateContainer)
        }
        if (delegateContainer.rowDelegate.visibleBottomBorderLast){
            bottomBorder.createObject(delegateContainer)
        }
        if (delegateContainer.rowDelegate.visibleLeftBorderFirst){
            leftBorder.createObject(delegateContainer)
        }
        if (delegateContainer.rowDelegate.visibleRightBorderLast){
            rightBorder.createObject(delegateContainer)
        }

        // let emptyDecorCell = delegateContainer.rowDelegate.tableItem.emptyDecorCell

        // let leftTopCornerPatchVisible = emptyDecorCell ? true :
        //                                             delegateContainer.rowDelegate.cellDecorator.IsValidData("LeftTopRound", delegateContainer.columnIndex) ?
        //                                             !delegateContainer.rowDelegate.cellDecorator.GetData("LeftTopRound", delegateContainer.columnIndex) :true
        // if (leftTopCornerPatchVisible){
        //     leftTopCornerPatch.createObject(mainRec)
        // }

        // let rightTopCornerPatchVisible = emptyDecorCell ? true :
        //                                             delegateContainer.rowDelegate.cellDecorator.IsValidData("RightTopRound", delegateContainer.columnIndex) ?
        //                                             !delegateContainer.rowDelegate.cellDecorator.GetData("RightTopRound", delegateContainer.columnIndex) :true
        // if (rightTopCornerPatchVisible){
        //     rightTopCornerPatch.createObject(mainRec)
        // }


        // let leftBottomCornerPatchVisible = emptyDecorCell ? true :
        //                                             delegateContainer.rowDelegate.cellDecorator.IsValidData("LeftBottomRound", delegateContainer.columnIndex) ?
        //                                             !delegateContainer.rowDelegate.cellDecorator.GetData("LeftBottomRound", delegateContainer.columnIndex) :true
        // if (leftBottomCornerPatchVisible){
        //     leftBottomCornerPatch.createObject(mainRec)
        // }

        // let rightBottomCornerPatchVisible = emptyDecorCell ? true :
        //                                             delegateContainer.rowDelegate.cellDecorator.IsValidData("RightBottomRound", delegateContainer.columnIndex) ?
        //                                             !delegateContainer.rowDelegate.cellDecorator.GetData("RightBottomRound", delegateContainer.columnIndex) :true
        // if (rightBottomCornerPatchVisible){
        //     rightBottomCornerPatch.createObject(mainRec)
        // }

        delegateContainer.rowDelegate.tableItem.widthRecalc.connect(delegateContainer.setCellWidth)
    }


    Component{
        id: topBorder;
        Rectangle{
            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.right: parent.right;
            height: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.visibleTopBorderFirst  ? delegateContainer.rowDelegate.horizontalBorderSize : 0 : 0;
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
            width: delegateContainer.rowDelegate ?
                       delegateContainer.rowDelegate.isRightBorder ? delegateContainer.rowDelegate.verticalBorderSize * delegateContainer.rowDelegate.visibleLeftBorderFirst * (delegateContainer.columnIndex == 0)
                                                                               : delegateContainer.rowDelegate.visibleLeftBorderFirst ? delegateContainer.rowDelegate.verticalBorderSize : delegateContainer.columnIndex > 0 ? delegateContainer.rowDelegate.verticalBorderSize : 0
                                                                               :0;
            color:  delegateContainer.rowDelegate ?delegateContainer.rowDelegate.borderColorVertical : "transparent";
        }
    }

    Component{
        Rectangle{
            id: rightBorder;
            anchors.right: parent.right;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
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
                                                                                                    delegateContainer.rowDelegate.cellDecorator.IsValidData("LeftTopRound", delegateContainer.columnIndex) ?
                                                                                                        !delegateContainer.rowDelegate.cellDecorator.GetData("LeftTopRound", delegateContainer.columnIndex) :true : 0;
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
                                                                                                    delegateContainer.rowDelegate.cellDecorator.IsValidData("RightTopRound", delegateContainer.columnIndex) ?
                                                                                                        !delegateContainer.rowDelegate.cellDecorator.GetData("RightTopRound", delegateContainer.columnIndex) :true : 0;
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
                                                                              delegateContainer.rowDelegate.cellDecorator.IsValidData("LeftBottomRound", delegateContainer.columnIndex) ?
                                                                                  !delegateContainer.rowDelegate.cellDecorator.GetData("LeftBottomRound", delegateContainer.columnIndex) :true : 0;
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
                                                                                                          delegateContainer.rowDelegate.cellDecorator.IsValidData("RightBottomRound", delegateContainer.columnIndex) ?
                                                                                                              !delegateContainer.rowDelegate.cellDecorator.GetData("RightBottomRound", delegateContainer.columnIndex) :true : 0;
        }
    }

    function getValue(){
            if (delegateContainer && delegateContainer.columnIndex >= 0){
                if(delegateContainer.rowDelegate !== null && delegateContainer.rowDelegate.tableItem !==null && delegateContainer.rowDelegate.dataModel != null){
                    return delegateContainer.rowDelegate.dataModel[delegateContainer.rowDelegate.tableItem.headers.GetData("Id", delegateContainer.columnIndex)];
                }
            }

            return "";
        }

    function setCellWidth(){
        if(!delegateContainer || !delegateContainer.rowDelegate){
            return;
        }

        var defaultWidth = delegateContainer.columnCount == 0 ? 0 : delegateContainer.rowDelegate.width/delegateContainer.columnCount;
        var widthFromModel = delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.IsValidData("Width", delegateContainer.columnIndex) ?
                    delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.GetData("Width", delegateContainer.columnIndex) : -1;

        if(!delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.GetItemsCount()){
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

