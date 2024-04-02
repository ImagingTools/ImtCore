import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableRowDelegateBase {
    id: tableDelegateContainer;

    TableRowViewer {
        anchors.fill: parent

        rowDelegate: tableDelegateContainer
        model: tableDelegateContainer.columnCount;
    }
}
