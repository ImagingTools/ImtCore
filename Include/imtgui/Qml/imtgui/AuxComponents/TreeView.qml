import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: Style.baseColor;

    property alias modelItems: mainTreeView.model;

    signal itemTreeViewCheckBoxStateChanged(int state, string packageId, string featureId);

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

            listViewItem: mainTreeView;

//            onCheckBoxStateChanged: {
//                console.log("TreeView ListView onCheckBoxStateChanged", state, packageId, featureId);
//                container.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
//            }

        }

        function changeCheckBoxState(state, packageId, featureId) {
            console.log("TreeView ListView onCheckBoxStateChanged()", state, packageId, featureId);
            container.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
        }

        onModelChanged: {
            console.log("TreeView ListView onModelChanged");
        }
    }
}
