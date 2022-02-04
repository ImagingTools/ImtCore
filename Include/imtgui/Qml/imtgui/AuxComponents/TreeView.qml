import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: Style.baseColor;

    property alias modelItems: mainTreeView.model;

    Component.onCompleted: {
    }

    onModelItemsChanged: {
        console.log("TreeView onModelItemsChanged");
    }

    TreeItemModel {
        id: treeItemModel;
    }

    ListView {
        id: mainTreeView;
        anchors.fill: container;
        delegate: TreeItemDelegate {
            width: parent.width;
            //childItemModel: model.childItemModel;
        }

        onModelChanged: {
            console.log("TreeView ListView onModelChanged");
        }
    }
}
