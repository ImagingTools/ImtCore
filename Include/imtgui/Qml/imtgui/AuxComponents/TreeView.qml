import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: Style.baseColor;

    Component.onCompleted: {
        var childItemModel;
        treeItemModel.SetIsArray(true);

        treeItemModel.InsertNewItem();
        treeItemModel.SetData("Name", "level0_0");
        treeItemModel.SetData("stateChecked", 0);
        treeItemModel.SetData("level", 0);
//        treeItemModel.SetData("isOpened", 0);

        childItemModel = treeItemModel.AddTreeModel("childItemModel");
        childItemModel.SetData("Name", "level1_0",0);
        childItemModel.SetData("stateChecked", 0, 0);
        childItemModel.SetData("parentIndex", 0, 0);
        childItemModel.SetData("level", 1, 0);
//        childItemModel.SetData("isOpened", 0, 0);

        childItemModel = childItemModel.AddTreeModel("childItemModel");
        childItemModel.SetData("Name", "level2_0",0);
        childItemModel.SetData("stateChecked", 0, 0);
        childItemModel.SetData("parentIndex", 0, 0);
        childItemModel.SetData("level", 2, 0);
//        childItemModel.SetData("isOpened", 0, 0);

//        console.log("Parent ", childItemModel.GetParent());

//        treeItemModel.InsertNewItem();
//        treeItemModel.SetData("Name", "level0_1",1);
//        treeItemModel.SetData("stateChecked", 0, 1);

//        childItemModel = treeItemModel.AddTreeModel("childItemModel", 1);
//        childItemModel.SetData("Name", "level1_1", 0);
//        childItemModel.SetData("stateChecked", 0, 0);
//        childItemModel.SetData("parentIndex", 1, 0);

//        childItemModel.InsertNewItem();
//        childItemModel.SetData("Name", "level1_2",1);
//        childItemModel.SetData("stateChecked", 0, 1);
//        childItemModel.SetData("parentIndex", 1, 1);

//        childItemModel.InsertNewItem();
//        childItemModel.SetData("Name", "level1_3", 2);
//        childItemModel.SetData("stateChecked", 0, 2);
//        childItemModel.SetData("parentIndex", 1, 2);

//        childItemModel = childItemModel.AddTreeModel("childItemModel");
//        childItemModel.SetData("Name", "level2_1", 0);
//        childItemModel.SetData("stateChecked", 2, 0);
//        childItemModel.SetData("parentIndex", 0, 0);
        mainTreeView.model = treeItemModel;
    }

    TreeItemModel {
        id: treeItemModel;
    }

//    Rectangle {
//        id: header;

//        width: parent.width;
//        anchors.topMargin: tabPanel.bottom;
//        anchors.leftMargin: container.left;
//        Text {
//            text: "Features";
//        }
//    }

//    Rectangle {
//        id: rect_split;
//        height: parent.height;
//        width: 5;
//        color: "#edefef";
//        anchors.left: container.left;
//        anchors.top: container.top;
//    }

    ListView {
        id: mainTreeView;
        anchors.fill: container;
        delegate: TreeItemDelegate {
            width: parent.width;
        }
    }
}
