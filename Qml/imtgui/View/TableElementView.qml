import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

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

            Component.onCompleted: {
                root.table = table;
            }
        }
    }
}


