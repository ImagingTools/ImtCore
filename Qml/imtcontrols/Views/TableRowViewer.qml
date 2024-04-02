import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Row {
    id: dataList;
    property alias model: repeater.model
    property TableRowDelegateBase rowDelegate: null;
    property bool compl: false;

    Repeater {
        id: repeater
        // orientation: ListView.Horizontal;

        // boundsBehavior: Flickable.StopAtBounds;

        // property TableBase table: null;




        Component.onCompleted: {
            dataList.compl = true;
        }

        //    delegate: table.columnContentComps[model.index];

        delegate: Item {
            id: cell
            property bool compl: false;
            property bool complCompl: dataList.compl && dataList.rowDelegate.table.columnCount;
            width: 20
            height: dataList.rowDelegate ? dataList.rowDelegate.height : 0;

            Component.onCompleted: {
                cell.compl = true;
            }

            onComplComplChanged: {
                if(cell.complCompl){
                    loader.sourceComponent = dataList.rowDelegate.table.columnContentComps[model.index] !== null ?
                                dataList.rowDelegate.table.columnContentComps[model.index] : dataList.rowDelegate.table.cellDelegate;

                    table.widthRecalc.connect(cell.setCellWidth)
                    cell.setCellWidth();
                }
            }

            function setCellWidth(){

                if(!cell.complCompl){
                    return;
                }

                var defaultWidth = dataList.rowDelegate.table.columnCount == 0 ? 0 : dataList.rowDelegate.table.width/dataList.rowDelegate.table.columnCount;
                var widthFromModel = dataList.rowDelegate.table.widthDecoratorDynamic.IsValidData("Width", model.index) ?
                            dataList.rowDelegate.table.widthDecoratorDynamic.GetData("Width", model.index) : -1;

                if(!dataList.rowDelegate.table.widthDecoratorDynamic.GetItemsCount()){
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
                        item.rowDelegate = dataList.rowDelegate
                        item.columnIndex = model.index
                    }
                }
            }
        }

        //    delegate: Component {
        //        Item {
        //            width: rowDelegate.table.width / rowDelegate.table.columnCount;
        //            height: parent.height;

        //            Text {
        //                id: contentText;

        //                width: parent.width;
        //            }

        //            Component.onCompleted: {
        //                let headerId = rowDelegate.table.headers.GetData("Id", model.index);
        //                let text = rowDelegate.table.elements.GetData(headerId, rowDelegate.rowIndex);
        //                contentText.text = text;
        //            }
        //        }
        //    }
    }
}


