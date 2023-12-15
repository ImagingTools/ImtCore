import QtQuick 2.12

import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: treeViewPage;

    anchors.fill: parent;

    TreeViewGql{
        anchors.centerIn: parent;

        width: parent.width - 200;
        height: parent.height - 200;
    }
}
