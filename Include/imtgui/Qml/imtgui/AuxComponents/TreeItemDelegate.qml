import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;
    width: 100;
    height: childrenColumn.visible ? mainRect.height + childrenColumn.height: mainRect.height;
    property bool isOpened: true;

    property TreeItemModel childItemModel;

    Component.onCompleted: {
        console.log("TreeItemDelegate Component.onCompleted");
        console.log("Model Name ", model.Name);
        console.log("Model level ", model.level);
        console.log("Model stateChecked ", model.stateChecked);
        if (model.childItemModel)
        {
            console.log("Model childCount ", model.childItemModel.GetItemsCount());
            treeItemRepeater.model = model.childItemModel;
        }
    }

    onChildItemModelChanged: {
        //treeItemRepeater.model = treeItemDelegate.childItemModel;
    }

    Rectangle {
        id: mainRect;
        width: parent.width - model.level * 20;
        color: Style.baseColor;
        anchors.top: parent.top;
        anchors.right: parent.right;
        height: 30;
//        color: "red";

        Image {
            id: iconArrow;
            width: 10;
            height: 10;
            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            visible: model.level === 0;
            source: treeItemDelegate.isOpened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                                "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                onClicked: {
                    treeItemDelegate.isOpened = !treeItemDelegate.isOpened;
                }
            }
        }

        CheckBox {
             id: checkBox;
             checkState: model.stateChecked;
             anchors.rightMargin: 10;
             anchors.right: titleModel.left;
             anchors.verticalCenter: parent.verticalCenter;
             visible: model.level === 1;
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
            anchors.left: iconArrow.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            text: model.Name;
            color: Style.textColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }
    }

    Column {
        id: childrenColumn;
        width: treeItemDelegate.width;
        anchors.top: mainRect.bottom;
        visible: treeItemDelegate.isOpened;
        height: treeItemRepeater.count * mainRect.height;

        Repeater {
             id: treeItemRepeater;
             delegate: Loader {
                 id: loader;
                 source: "TreeItemDelegate.qml";
             }
       }
    }
}
