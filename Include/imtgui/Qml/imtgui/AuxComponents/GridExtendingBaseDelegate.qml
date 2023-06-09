import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0

Rectangle {
    id: gridExtendingBaseDelegate;

    anchors.top: parent.top;
    anchors.topMargin: !rootItem ? O :rootItem.gridCellHeightMin * rowNumber  + rootItem.gridAddHeight * rootItem.openST * belowSelectedRow;

    color: "transparent";

    property Item rootItem: null;

    property bool selected: !rootItem ? false : rootItem.selectedIndex === model.index;

    property int rowNumber: !rootItem ? O : Math.trunc(model.index/rootItem.gridCountInLine);
    property bool belowSelectedRow: !rootItem ? false : model.index >= (rootItem.gridSelectedRow + 1) * rootItem.gridCountInLine;
    property bool inLastRow: !rootItem ? false : model.index >= (rootItem.gridRowCount - 1) * rootItem.gridCountInLine;

    function setOpenST(){
        if(gridExtendingBaseDelegate.rootItem){

            if(model.index == rootItem.selectedIndex){
                rootItem.openST = !rootItem.openST;
            }
            else{
                rootItem.openST = false;
                rootItem.openST = true;
            }
        }
    }

}

