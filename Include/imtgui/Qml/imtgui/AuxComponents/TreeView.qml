import QtQuick 2.0
import Acf 1.0

Rectangle {
    id: container;
    color: "white";

    Component.onCompleted: {
        console.log("Tree view completed!");
        var childModel;
        treeModel.SetIsArray(true);

        treeModel.InsertNewItem();
        treeModel.SetData("Name", "level0_0");
        treeModel.SetData("stateChecked", Qt.Unchecked);

       // treeModel.AddTreeModel2("ChildModel",0);
        childModel = treeModel.AddTreeModel("ChildModel");
        childModel.SetData("Name", "level1_0",0);
        childModel.SetData("stateChecked", Qt.Unchecked, 0);
        childModel.SetData("parentIndex", 0, 0);

        childModel = childModel.AddTreeModel("ChildModel");
        childModel.SetData("Name", "level2_0",0);
        childModel.SetData("stateChecked", Qt.Unchecked, 0);
        childModel.SetData("parentIndex", 0, 0);

        console.log("Parent ", childModel.GetParent());

        treeModel.InsertNewItem();
        treeModel.SetData("Name", "level0_1",1);
        treeModel.SetData("stateChecked", Qt.Unchecked, 1);

        childModel = treeModel.AddTreeModel("ChildModel", 1);
        childModel.SetData("Name", "level1_1", 0);
        childModel.SetData("stateChecked", Qt.Unchecked, 0);
        childModel.SetData("parentIndex", 1, 0);

        childModel.InsertNewItem();
        childModel.SetData("Name", "level1_2",1);
        childModel.SetData("stateChecked", Qt.Unchecked, 1);
        childModel.SetData("parentIndex", 1, 1);

        childModel.InsertNewItem();
        childModel.SetData("Name", "level1_3", 2);
        childModel.SetData("stateChecked", Qt.Unchecked, 2);
        childModel.SetData("parentIndex", 1, 2);

//        childModel.InsertNewItem();
//        childModel = treeModel.AddTreeModel("ChildModel", 1);
//        childModel.SetData("Name", "level1_2", 1);

//        childModel.InsertNewItem();
//        childModel = treeModel.AddTreeModel("ChildModel", 1);
//        childModel.SetData("Name", "level1_3", 2);
       // var childModel2;
        childModel = childModel.AddTreeModel("ChildModel");
        childModel.SetData("Name", "level2_1", 0);
        childModel.SetData("stateChecked", Qt.Checked, 0);
        childModel.SetData("parentIndex", 0, 0);

//        console.log("ChildModel ", childModel.GetTreeItemModel("ChildModel", 0));

//        console.log("AddTreeModel succesfully!!!");
//        console.log("GetData",treeModel.GetData("Name", 0));
//        console.log("AddTreeModel succesfully!!!",treeModel.GetData("Name", 1));
//        console.log("AddTreeModel succesfully!!!",treeModel.GetItemsCount());
        treeView.model = treeModel;
    }

    ListView {
        id: treeView;
        anchors.fill: container;
        delegate: TreeItemDelegate {
            width: treeView.width;
        }
    }

    TreeItemModel {
        id: treeModel;
    }

    Component {
            id: compTreeItemDelegate
            Item {
                id: treeitem;
                width: treeView.width;
                property bool isOpened: false;
                property int level: 1;

              //  height: isOpened ? 30 + childListView.height : 30;
                height: 30 + childListView.height;

                Rectangle {
                    anchors.fill: parent;

                   // width: (level > 5 ? 6: level ) * 32 + 5;

                    Text {
                        id: titleModel;
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        text: modelName;
                        color: "black"
                    }

                    MouseArea {
                        id: mouseArea;
                        anchors.fill: parent;

                        onClicked: {
                            console.log("Model onClicked!");
                            if (!isOpened)
                            {
                                childListView.model = modelChildModel;
                                console.log("Count child ", childListView.count);
                            }
                            isOpened = !isOpened;
                        }
                    }
                }

                ListView {
                    id: childListView;
                    width: parent.width;
                    height: childListView.count * 30;
                    visible: isOpened;
                  //  anchors.top: treeitem.bottom;
                    delegate: comp2;
                }
            }
    }

}
