import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: Style.baseColor;

    property alias modelItems: mainTreeView.model;

    Component.onCompleted: {
//        var childItemModel;
//        treeItemModel.SetIsArray(true);

//        treeItemModel.InsertNewItem();
//        treeItemModel.SetData("Name", "Feature 1");
//        treeItemModel.SetData("stateChecked", 0);
//        treeItemModel.SetData("level", 0);

//        childItemModel = treeItemModel.AddTreeModel("childItemModel");
//        childItemModel.SetData("Name", "A",0);
//        childItemModel.SetData("stateChecked", 0, 0);
//        childItemModel.SetData("level", 1, 0);

//        childItemModel.InsertNewItem();
//        childItemModel.SetData("Name", "B", 1);
//        childItemModel.SetData("stateChecked", 0, 1);
//        childItemModel.SetData("level", 1, 1);

//        var index = treeItemModel.InsertNewItem();
//        treeItemModel.SetData("Name", "Feature 2", index);
//        treeItemModel.SetData("stateChecked", 0, index);
//        treeItemModel.SetData("level", 0, index);

//        childItemModel = treeItemModel.AddTreeModel("childItemModel", 1);
//        childItemModel.SetData("Name", "A", 1);
//        childItemModel.SetData("stateChecked", 0, 1);
//        childItemModel.SetData("level", 1, 1);
//        mainTreeView.model = treeItemModel;
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
