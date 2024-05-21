import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root;

    property BasicTreeView treeView;

    bottomComp: treeViewComp;

    Component {
        id: treeViewComp;

        BasicTreeView {
            id: treeView;

            width: root.contentWidth;
            height: 600;

            headerVisible: false;

            Component.onCompleted: {
                root.treeView = treeView;
            }
        }
    }
}


