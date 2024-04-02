import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Item {
    id: delegateContainer;

    property TableRowDelegateBase rowDelegate: null;

    height: rowDelegate ? rowDelegate.height : 0;
    width: rowDelegate ? rowDelegate.width/rowDelegate.columnCount : 0;

    property int columnCount: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.columnCount : 0;

    property real textLeftIndent: 0
    property real textRightIndent: 0

    property int columnIndex: -1
    property int rowIndex: rowDelegate ? rowDelegate.rowIndex : -1;


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

        // let emptyDecorCell = delegateContainer.rowDelegate.table.emptyDecorCell

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
    }


    Rectangle{
        id: mainRec;

        anchors.fill: parent;

        color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.table.emptyDecorCell ? "transparent" :
                                                                          delegateContainer.rowDelegate.table.cellDecorator.IsValidData("Color", delegateContainer.columnIndex) ?
                                                                              delegateContainer.rowDelegate.table.cellDecorator.GetData("Color", delegateContainer.columnIndex) :
                                                                              "transparent" : "transparent"

        opacity: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.table.emptyDecorCell ? 1 :
                                                                            delegateContainer.rowDelegate.table.cellDecorator.IsValidData("Opacity", delegateContainer.columnIndex) ?
                                                                                delegateContainer.rowDelegate.table.cellDecorator.GetData("Opacity", delegateContainer.columnIndex) :
                                                                                1 : 0;

        radius: delegateContainer.rowDelegate ?  delegateContainer.rowDelegate.table.emptyDecorCell ? 0 :
                                                                           delegateContainer.rowDelegate.table.cellDecorator.IsValidData("CellRadius", delegateContainer.columnIndex) ?
                                                                               delegateContainer.rowDelegate.table.cellDecorator.GetData("CellRadius", delegateContainer.columnIndex) :0 : 0;

    }//mainRec

    Rectangle {
        id: selectionRec;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.selectedColor : "transparent";
        opacity: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.selectedOpacity : 0;

        radius: mainRec.radius;
        visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.selected && mainRec.color != "transparent" : false;
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
            visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.table.emptyDecorCell ? true :
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
            visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.table.emptyDecorCell ? true :
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
                         delegateContainer.rowDelegate.table.emptyDecorCell ? true :
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
            visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.table.emptyDecorCell ? true :
                                                                                                          delegateContainer.rowDelegate.cellDecorator.IsValidData("RightBottomRound", delegateContainer.columnIndex) ?
                                                                                                              !delegateContainer.rowDelegate.cellDecorator.GetData("RightBottomRound", delegateContainer.columnIndex) :true : 0;
        }
    }

    function getValue(){
            if (delegateContainer.columnIndex >= 0){
                if(delegateContainer.rowDelegate !== null && delegateContainer.rowDelegate.table !==null){
                    return delegateContainer.rowDelegate.dataModel[delegateContainer.rowDelegate.table.headers.GetData("Id", delegateContainer.columnIndex)];
                }
            }

            return "";
        }


}//delegate

