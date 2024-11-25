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
            property bool complCompl: dataList.compl && dataList && dataList.rowDelegate.tableItem.columnCount;
            width: 20
            height: dataList.height;

			clip: true;

            Component.onCompleted: {
                cell.compl = true;
            }

            Component.onDestruction: {
                if (dataList && dataList.rowDelegate && dataList.rowDelegate.tableItem){
                    dataList.rowDelegate.tableItem.widthRecalc.disconnect(cell.setCellWidth)
                }
            }

            onComplComplChanged: {
                if(cell.complCompl && dataList && dataList.rowDelegate && dataList.rowDelegate.tableItem){
                    let headerId = dataList.rowDelegate.tableItem.headers.getData("Id", model.index)
                    let contents = dataList.rowDelegate.tableItem.columnContentComps;
                    let contentComp = dataList.rowDelegate.tableItem.cellDelegate;
                    if (Object.keys(contents).includes(headerId)){
                        if (contents[headerId]){
                            contentComp = contents[headerId];
                        }
                    }

                    loader.sourceComponent = contentComp;

                    dataList.rowDelegate.tableItem.widthRecalc.connect(cell.setCellWidth)
                    cell.setCellWidth();
                }
            }

            function setCellWidth(){
                if(!dataList.rowDelegate || !dataList.rowDelegate.tableItem || !dataList){
                    return;
                }

                var defaultWidth = dataList.rowDelegate.tableItem.columnCount == 0 ? 0 : dataList.rowDelegate.tableItem.width/dataList.rowDelegate.tableItem.columnCount;
                var widthFromModel = dataList.rowDelegate.tableItem.widthDecoratorDynamic.isValidData("Width", model.index) ?
                            dataList.rowDelegate.tableItem.widthDecoratorDynamic.getData("Width", model.index) : -1;

                if(!dataList.rowDelegate.tableItem.widthDecoratorDynamic.getItemsCount()){
                    cell.width = defaultWidth;
                }
                else if(widthFromModel >= 0){
                    cell.width = widthFromModel;
                }
                else{
                    cell.width = defaultWidth;
                }

                cell.visible = cell.width > 0;
            }

            Loader {
                id: loader;
                anchors.fill: parent
                onItemChanged: {
                    if (item && dataList){
                        item.columnIndex = model.index
                        item.rowDelegate = dataList.rowDelegate
                    }
                }
            }
        }
    }
}


