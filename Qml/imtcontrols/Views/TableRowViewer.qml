import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Row {
    id: dataList;
    property TableRowDelegateBase rowDelegate: null;
    property bool compl: false;
    property alias model: repeater.model


    Repeater {
        id: repeater

        Component.onCompleted: {
            dataList.compl = true;
        }

        delegate: Item {
            id: cell
            property bool compl: false;
            property bool complCompl: dataList.compl && dataList.rowDelegate.tableItem.columnCount;
            width: 20
            height: dataList.height;

            Component.onCompleted: {
                cell.compl = true;
            }

            Component.onDestruction: {
                // console.log("*Debug* cell onDestruction")
                dataList.rowDelegate.tableItem.widthRecalc.disconnect(cell.setCellWidth)
            }

            onComplComplChanged: {
                if(cell.complCompl){
                    loader.sourceComponent = dataList.rowDelegate.tableItem.columnContentComps[model.index] !== null ?
                                dataList.rowDelegate.tableItem.columnContentComps[model.index] : dataList.rowDelegate.tableItem.cellDelegate;

                    dataList.rowDelegate.tableItem.widthRecalc.connect(cell.setCellWidth)
                    cell.setCellWidth();
                }
            }

            function setCellWidth(){

                if(!dataList.rowDelegate || !dataList.rowDelegate.tableItem){
                    return;
                }

                var defaultWidth = dataList.rowDelegate.tableItem.columnCount == 0 ? 0 : dataList.rowDelegate.tableItem.width/dataList.rowDelegate.tableItem.columnCount;
                var widthFromModel = dataList.rowDelegate.tableItem.widthDecoratorDynamic.IsValidData("Width", model.index) ?
                            dataList.rowDelegate.tableItem.widthDecoratorDynamic.GetData("Width", model.index) : -1;

                if(!dataList.rowDelegate.tableItem.widthDecoratorDynamic.GetItemsCount()){
                    cell.width = defaultWidth;
                }
                else if(widthFromModel >= 0){
                    cell.width = widthFromModel;
                }
                else{
                    cell.width = defaultWidth;
                }
            }

            Loader {
                id: loader;
                anchors.fill: parent
                onItemChanged: {
                    if (item){
                        item.columnIndex = model.index
                        item.rowDelegate = dataList.rowDelegate
                    }
                }
            }
        }
    }
}


