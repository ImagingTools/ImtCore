import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;
    width: 100;
    height: childrenColumn.visible ? mainRect.height + childrenColumn.height: mainRect.height;
    property bool isOpened: false;
   // property var currentColumn;
//    property int level: 0;

//    function changeStateAllChildren (root, model, state)
//    {
//        if (!model)
//        {
//            return;
//        }

//        var countChildren = model.GetItemsCount();
//        console.log("countChildren", countChildren);
//        for (var i = 0; i < countChildren; i++)
//        {

//            console.log(model.GetData("Name", i));
//            console.log(model);
//            model.SetData("stateChecked", state, i);
//            console.log(model.GetData("stateChecked", i));
//            var childModel = model.GetData("ChildModel", i);
//            changeStateAllChildren(root, childModel, state);
//        }
//    }
//    Rectangle {
//        anchors.fill: parent;
//        color: Style.mainColor;
//    }

    Component.onCompleted: {
        if (model.childItemModel)
        {
            treeItemRepeater.model = model.childItemModel;
        }
    }

    Rectangle {
        id: mainRect;
        width: parent.width - model.level * 20;
        //width: parent.width;
        color: Style.baseColor;
        anchors.top: parent.top;
        anchors.right: parent.right;
        height: 30;

        Rectangle {
            id: buttonClick;
            width: 10;
            height: width;
            //visible: treeItemRepeater.count > 0;
            anchors.leftMargin: 10;
            anchors.left: checkBox.right;
            anchors.verticalCenter: parent.verticalCenter;
            color: "transparent";
            Text {
                id: znak;
                text: treeItemDelegate.isOpened ? "-" : "+";
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                onClicked: {
                    console.log("================Model onClicked!================");
                    treeItemDelegate.isOpened = !treeItemDelegate.isOpened;
                      //console.log("treeItemDelegate.isOpened", model.isOpened);
                     // model.isOpened = 1;
                       //treeItemDelegate.changeStatusVisible();
                      //changeStatusVisible();
                      //console.log("treeItemDelegate.isOpened", model.isOpened);
//                    if (model.isOpened === 0)
//                    {
//                        model.isOpened = 1;
//                    } else
//                    {
//                        model.isOpened = 0;
//                    }

//                    model.SetData("isOpened", !model.isOpened);
//                    childrenColumn.visible = !childrenColumn.visible;
//                    console.log("titleModel text", model.Name);
//                    console.log("treeItemDelegate level", treeItemDelegate.level);
//                    console.log("treeItemRepeater.count", treeItemRepeater.count);
//                    console.log("treeItemDelegate.isOpened", model.isOpened);
//                    console.log("znak text", znak.text);
//                    console.log("*****Column, repeater!*****");
//                    console.log("childrenColumn width", childrenColumn.width);
//                    console.log("childrenColumn height", childrenColumn.height);
//                    console.log("childrenColumn visible", childrenColumn.visible);
//                    console.log("treeItemRepeater.count", treeItemRepeater.count);
//                    console.log("childrenColumn.height", childrenColumn.height);
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
             }

             MouseArea {
                 anchors.fill: parent;

                 onClicked: {
                     checkBox.checkState == 2 ? checkBox.checkState = 0 : checkBox.checkState = 2;
                 }
             }
        }

        Text {
            id: titleModel;
            anchors.left: buttonClick.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            text: model.Name;
            color: Style.textColor;
        }
    }

    Column {
        id: childrenColumn;
        width: treeItemDelegate.width;
//        visible: model.isOpened;
        anchors.top: mainRect.bottom;
        visible: treeItemDelegate.isOpened;
        //anchors.bottom: treeItemDelegate.bottom;
        height: treeItemRepeater.count * mainRect.height;

       // property bool visibleTest: false;

        Repeater {
             id: treeItemRepeater;
             delegate: Loader {
                 id: loader;
//                 source: "AuxComponents/TreeItemDelegate.qml";
                 source: "TreeItemDelegate.qml";

//                 Component.onCompleted: {
//                     loader.item.level = treeItemDelegate.level + 1;
//                 }
             }
       }
    }
}
