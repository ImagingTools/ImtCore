import QtQuick 2.15
import Acf 1.0

Item {
    id: treeViewContainer;

    property alias headers: table.headers;
    property alias elements: table.elements;

    property alias table: table;

    property ModelIndex selectedIndex: null;

    Timer {
        id: timer;

        interval: 100;

        onTriggered: {
            selectedIndex = null;
        }
    }

    Component {
        id: delegateComp;

        TableTreeItemDelegate {
            id: delegate;

            width: treeViewContainer.width;

            onClicked: {
                if (delegate.modelIndex.equal(selectedIndex)){
                    timer.start();
                }
                else{
                    selectedIndex = modelIndex;
                }
            }

            onDoubleClicked: {
                timer.stop();
            }
        }
    }

    AuxTable {
        id: table;

        width: parent.width;
        height: parent.height;

        delegate: delegateComp;
    }
}
