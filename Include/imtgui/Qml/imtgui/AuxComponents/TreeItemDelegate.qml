import QtQuick 2.0

Item {
    id: treeItemDelegate;
    width: parent.width;
    height: isOpened ? mainRect.height + childrenColumn.height: mainRect.height;
    property bool isOpened: false;
    property int level: 0;

    //property int stateCheckBox: checkBox.checkState;

//    function changeStateAllChildren (root, model, index, state)
//    {
//        /*Обход по ветке по индексу*/
//        var item;
//        console.log(root.GetData("Name", index));
//        item = root.GetTreeItemModel("ChildModel", index);
//        root.SetData("stateChecked", state, index);
//       // console.log(item.GetData("Name", index));
//        if (!item)
//        {
//            return;
//        }
//        var countChildren = item.GetItemsCount();
//        console.log("countChildren", countChildren);
//        for (var i = 0; i < countChildren; i++)
//        {
//            changeStateAllChildren(item, model, i, state);
//        }
//    }

//    function changeStateAllChildren (root, model, index, state)
//    {
//        if (!root)
//        {
//            return;
//        }

//        var countChildren = root.GetItemsCount();
//        console.log(root.GetData("Name", index));
//       // console.log(root.GetData("Name", 1));
//        for (var i = 0; i < countChildren; i++)
//        {
//            var item = root.GetTreeItemModel("ChildModel", i);
//            //console.log(item.GetData("Name", i));
//            changeStateAllChildren(item, model, i, state);
//        }
//    }

    function findModel(root, curModel)
    {
            var item;
            console.log(root.GetData("Name", index));
            item = root.GetTreeItemModel("ChildModel", index);
            root.SetData("stateChecked", state, index);
            if (!item)
            {
                return;
            }
            var countChildren = item.GetItemsCount();
            console.log("countChildren", countChildren);
            for (var i = 0; i < countChildren; i++)
            {
                changeStateAllChildren(item, model, i, state);
            }
    }

    function changeStateAllChildren (root, model, state)
    {
//        console.log(model)
//        console.log(model.GetData("Name"))
//        console.log(model.GetItemsCount())

        if (!model)
        {
            return;
        }

        var countChildren = model.GetItemsCount();
        console.log("countChildren", countChildren);
        for (var i = 0; i < countChildren; i++)
        {

            console.log(model.GetData("Name", i));
            console.log(model)
            model.SetData("stateChecked", state, i);
            console.log(model.GetData("stateChecked", i));
            var childModel = model.GetData("ChildModel", i);
            changeStateAllChildren(root, childModel, state);
//            if (childModel)
//            {
//                changeStateAllChildren(root, childModel, state);
//            }
        }

//        if (root === model)
//        {
//            console.log("Model equal !");
//        }

//        var countChildren = root.GetItemsCount();
//        for (var i = 0; i < countChildren; i++)
//        {
//            console.log(root.GetData("Name", i));
//            changeStateAllChildren(root.GetData("ChildModel", i), model, state);
//        }
    }

    Component.onCompleted: {
        repeater.model = model.ChildModel;
    }

    Rectangle {
        id: mainRect;
        width: parent.width - level * 20;
        anchors.top: parent.top;
        anchors.right: parent.right;
        height: 30;

        Text{
            id: buttonClick;
            width: 10;
            height: width;
            visible: repeater.count > 0;
            text: isOpened ?  "-" : "+";
            anchors.leftMargin: 10;
            anchors.left: checkBox.right;
            anchors.verticalCenter: parent.verticalCenter;
            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                onClicked: {
                    console.log("Model onClicked!");
                    isOpened = !isOpened;
                   // changeStateAllChildren(treeModel, model.index, checkBox.checkState);
                }
            }
        }

        CheckBox {
             id: checkBox;
             checkState: model.stateChecked;
             anchors.left: parent.left;
             anchors.verticalCenter: parent.verticalCenter;

             onCheckStateChanged: {
                  console.log("Check state ", checkBox.checkState);
               //  console.log("model.ChildModel ", model.ChildModel.GetItemsCount());
                // model.stateChecked = checkBox.checkState;
                // changeStateAllChildren(treeModel, model.index, checkBox.checkState);
//                 var item = repeater.itemAt(0).item;
//                 console.log("До ", stateCheckBox ,item.stateCheckBox);
//              //   item.stateCheckBox = stateCheckBox;
//                 console.log("После ", stateCheckBox, item.stateCheckBox);

                 //stateCheckBox = Qt.Unchecked;
                 //treeItemDelegate.changeStateForChild(checkState);
//                 for (var i = 0; i < repeater.count; i++)
//                 {
//                     var item = repeater.itemAt(i).item;
//                    //  console.log(item.model.Name);
//                     item.stateCheckBox = stateCheckBox;
//                 }
             }

             MouseArea {
                 anchors.fill: parent;

                 onClicked: {
                     checkBox.checkState == Qt.Checked ? checkBox.checkState = Qt.Unchecked : checkBox.checkState = Qt.Checked;
                     //changeStateAllChildren(model, model.index, model.parentIndex, checkBox.checkState, level);
                     changeStateAllChildren(treeModel, repeater.model, checkBox.checkState);
                 }
             }
        }

        Text {
            id: titleModel;
            anchors.left: buttonClick.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            text: model.Name;
        }
    }

    Column {
        id: childrenColumn;
        width: parent.width;
        visible: isOpened;
        anchors.top: mainRect.bottom;
        Repeater {
             id: repeater;
             width: parent.width;
             delegate: Loader {
                 id: loader;
//                 source: "TreeItemDelegate.qml";
                 Component.onCompleted: {
//                     loader.item.level = treeItemDelegate.level + 1;
                      setSource("TreeItemDelegate.qml", { "level": level + 1, "width" : treeItemDelegate.width })
                 }
             }

//              delegate: Loader {
//                     id: loader;
//                     source: "TreeItemDelegate.qml";
//                     Component.onCompleted {
//                         loader.item.level = level + 1;
//                        // loader.setSource("TreeItemDelegate.qml", { "level": level + 1, "width" : treeItemDelegate.width })
//                     }
//                 }
        }
    }
}
