import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property TreeItemModel modelItems;

    Component {
        id: delegateComp;

        TreeItemDelegate {
            width: treeViewContainer.width;
        }
    }

    ListView {
        id: mainTreeView;

        anchors.fill: treeViewContainer;

        boundsBehavior: Flickable.StopAtBounds;
        model: treeViewContainer.modelItems;

        delegate: delegateComp;
    }
}
