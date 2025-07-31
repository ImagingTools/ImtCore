import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/*
    TableElementView {
        id: tableElement;
        width: parent.width;
        name: qsTr("Table");
        onTableChanged: {
            if (table){
                table.checkable = true;
                // ...
            }
        }

        Connections {
            target: tableElement.table;

            function onCheckedItemsChanged(){
                // ...
            }
        }
    }
*/


ElementView {
    id: root;

    bottomComp: tableComp;

    property Table table;

    Component {
        id: tableComp;

        Table {
            id: table;

            width: root.contentWidth;
            height: contentHeight + headerHeight;
            canMoveColumns: true

            Component.onCompleted: {
                root.table = table;
            }
        }
    }
}


