import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: "white";

    Component.onCompleted: {
        console.log("Tree view completed!");
        var childItemModel;
        treeItemModel.SetIsArray(true);

        treeItemModel.InsertNewItem();
        treeItemModel.SetData("Name", "level0_0");
        treeItemModel.SetData("stateChecked", 0);

       // treeItemModel.AddTreeModel2("childItemModel",0);
        childItemModel = treeItemModel.AddTreeModel("childItemModel");
        childItemModel.SetData("Name", "level1_0",0);
        childItemModel.SetData("stateChecked", 0, 0);
        childItemModel.SetData("parentIndex", 0, 0);

        childItemModel = childItemModel.AddTreeModel("childItemModel");
        childItemModel.SetData("Name", "level2_0",0);
        childItemModel.SetData("stateChecked", 0, 0);
        childItemModel.SetData("parentIndex", 0, 0);

        console.log("Parent ", childItemModel.GetParent());

        treeItemModel.InsertNewItem();
        treeItemModel.SetData("Name", "level0_1",1);
        treeItemModel.SetData("stateChecked", 0, 1);

        childItemModel = treeItemModel.AddTreeModel("childItemModel", 1);
        childItemModel.SetData("Name", "level1_1", 0);
        childItemModel.SetData("stateChecked", 0, 0);
        childItemModel.SetData("parentIndex", 1, 0);

        childItemModel.InsertNewItem();
        childItemModel.SetData("Name", "level1_2",1);
        childItemModel.SetData("stateChecked", 0, 1);
        childItemModel.SetData("parentIndex", 1, 1);

        childItemModel.InsertNewItem();
        childItemModel.SetData("Name", "level1_3", 2);
        childItemModel.SetData("stateChecked", 0, 2);
        childItemModel.SetData("parentIndex", 1, 2);

//        childItemModel.InsertNewItem();
//        childItemModel = treeModel.AddTreeModel("childItemModel", 1);
//        childItemModel.SetData("Name", "level1_2", 1);

//        childItemModel.InsertNewItem();
//        childItemModel = treeModel.AddTreeModel("childItemModel", 1);
//        childItemModel.SetData("Name", "level1_3", 2);
       // var childItemModel2;
        childItemModel = childItemModel.AddTreeModel("childItemModel");
        childItemModel.SetData("Name", "level2_1", 0);
        childItemModel.SetData("stateChecked", 2, 0);
        childItemModel.SetData("parentIndex", 0, 0);

//        console.log("childItemModel ", childItemModel.GetTreeItemModel("childItemModel", 0));

//        console.log("AddTreeModel succesfully!!!");
//        console.log("GetData",treeModel.GetData("Name", 0));
//        console.log("AddTreeModel succesfully!!!",treeModel.GetData("Name", 1));
//        console.log("AddTreeModel succesfully!!!",treeModel.GetItemsCount());
        treeView.model = treeItemModel;
    }

    TreeItemModel {
        id: treeItemModel;
    }

    ListView {
        id: treeView;
        anchors.fill: container;
        delegate: TreeItemDelegate {
            width: parent.width;
        }
    }



}
