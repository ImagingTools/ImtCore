import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

TreeViewItemDelegateBase {
    id: tableViewDelegate;

    root: permissionsTable;

    prefixRowDelegate: Item {
        width: checkBox.visible ? 25 : 0;
        height: tableViewDelegate.root ? tableViewDelegate.root.rowItemHeight: 0;

        CheckBox {
            id: checkBox;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.horizontalCenter: parent.horizontalCenter;

            checkState: model.CheckState;

            onClicked: {
                model.CheckState = Qt.Checked - model.CheckState;

                tableViewDelegate.root.rowModelDataChanged(tableViewDelegate, "CheckState");
            }
        }
    }
}
