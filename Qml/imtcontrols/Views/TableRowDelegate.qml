import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableRowDelegateBase {
    id: tableDelegateContainer;

    TableRowViewer {
        anchors.fill: parent
        anchors.leftMargin: tableDelegateContainer.table && tableDelegateContainer.table.checkable ? 2* Style.size_mainMargin : 0;

        rowDelegate: tableDelegateContainer
        model: tableDelegateContainer.columnCount;
    }
}
