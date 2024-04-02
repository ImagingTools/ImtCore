import QtQuick 2.12
import Acf 1.0


TableCellDelegateBase {
    id: delegateContainer;

    TableCellText {
        anchors.left: parent.left;
        anchors.leftMargin:  rowDelegate.textLeftMargin  // .textLeftIndent;
        anchors.right: parent.right;
        anchors.rightMargin: rowDelegate.textRightMargin;
        anchors.verticalCenter: parent.verticalCenter

        // text: delegateContainer.columnIndex + " " + delegateContainer.rowIndex //getValue()
        rowDelegate:  delegateContainer.rowDelegate
        text: getValue()
    }
}

