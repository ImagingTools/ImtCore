import QtQuick 2.12
import Acf 1.0

TableCellDelegateBase {
    id: delegateContainer

    signal clicked();

    TableCellText {
        id: tableCellText;

        anchors.left: parent.left
        anchors.leftMargin: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textLeftMargin : 0
        anchors.right: parent.right
        anchors.rightMargin: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textRightMargin : 0
        anchors.verticalCenter: parent.verticalCenter

        rowDelegate: delegateContainer.rowDelegate
        text: delegateContainer.getValue()
        columnIndex: delegateContainer.columnIndex
    }

    TooltipArea{
        text: tableCellText.text;
        mouseArea: ma;
    }

    MouseArea{
        id: ma;
        anchors.fill: parent;
        property Item tableItem:  delegateContainer.rowDelegate && delegateContainer.rowDelegate.tableItem ? delegateContainer.rowDelegate.tableItem : null;


        onClicked: {
            if(tableItem && tableItem.elements.getItemsCount() && delegateContainer.rowIndex > -1){
                tableItem.tableSelection.singleSelect(delegateContainer.rowIndex)
                delegateContainer.clicked();
            }
        }
    }

}
