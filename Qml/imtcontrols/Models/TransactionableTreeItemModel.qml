import QtQuick 2.0
import Acf 1.0

TreeItemModel {
    id: root;

    onDataChanged: {
        if (internal.transaction){
            return;
        }
    }

    function beginTransaction(){
        internal.transaction = true;
    }

    function endTransaction(){
        internal.transaction = false;

        root.dataChanged();
    }

    QtObject {
        id: internal;

        property bool transaction: false;
    }
}
