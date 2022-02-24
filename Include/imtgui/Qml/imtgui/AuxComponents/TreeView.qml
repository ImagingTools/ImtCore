import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: treeViewContainer;
    color: Style.baseColor;

//    property alias modelItems: mainTreeView.model;
    property var modelItems;

    signal itemTreeViewCheckBoxStateChanged(int state, string packageId, string featureId);

    onModelItemsChanged: {
//        console.log("TreeView onModelItemsChanged");

//        if (treeViewContainer.modelItems){
//            console.log("onModelItemsChanged", treeViewContainer.modelItems.GetItemsCount());
//            for (var i = 0; i < treeViewContainer.modelItems.GetItemsCount(); i++){
//                console.log("Package Id: ", treeViewContainer.modelItems.GetData("Id", i));
//            }

//        }
    }

    TreeItemModel {
        id: treeItemModel;
    }

    ListView {
        id: mainTreeView;
        anchors.fill: treeViewContainer;
        boundsBehavior: Flickable.StopAtBounds;

        model: treeViewContainer.modelItems;

        delegate: TreeItemDelegate {
            width: parent.width;
            //childItemModel: model.childItemModel;

            listViewItem: mainTreeView;

            onCheckBoxState: {
                console.log("TreeView ListView onCheckBoxStateChanged", state, packageId, featureId);
                treeViewContainer.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
            }

//            onCheckBoxStateChanged: {
//                console.log("TreeView ListView onCheckBoxStateChanged", state, packageId, featureId);
//                treeViewContainer.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
//            }

//            onCheckBoxStateChangedChanged: {
//                console.log("TreeView ListView onCheckBoxStateChanged", state, packageId, featureId);
//                treeViewContainer.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
//            }

        }

        function changeCheckBoxState(state, packageId, featureId) {
            console.log("TreeView ListView onCheckBoxStateChanged()", state, packageId, featureId);
            treeViewContainer.itemTreeViewCheckBoxStateChanged(state, packageId, featureId);
        }

        onModelChanged: {
            console.log("TreeView ListView onModelChanged");
        }
    }
}
