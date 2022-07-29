import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property TreeItemModel modelItems;

    signal checkBoxStateChanged(int state, string parentId, string childId);

    ListView {
        id: mainTreeView;

        anchors.fill: treeViewContainer;

        boundsBehavior: Flickable.StopAtBounds;
        model: treeViewContainer.modelItems;

        property int currentParentIndex: -1;
        property int currentChildIndex: -1;

        delegate: TreeItemDelegate {
            width: parent.width;
        }
    }
}
