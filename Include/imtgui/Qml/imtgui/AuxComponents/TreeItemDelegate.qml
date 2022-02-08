import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;
    width: 100;
    height: childrenColumn.visible ? mainRect.height + childrenColumn.height: mainRect.height;
//    height:  mainRect.height + childrenColumn.height;
    property bool isOpened: true;

    property TreeItemModel childItemModel;

    property Item listViewItem;

//    signal checkBoxStateChanged(int state, string packageId, string featureId);

    Component.onCompleted: {
        if (model.childItemModel)
        {
            console.log("Model childCount ", model.childItemModel.GetItemsCount());
            treeItemRepeater.model = model.childItemModel;
        }
    }

//    onCheckBoxStateChanged: {
//        //treeItemRepeater.model = treeItemDelegate.childItemModel;
//    }

    Rectangle {
        id: mainRect;
        width: parent.width - model.level * 20;
        color: Style.baseColor;
        anchors.top: parent.top;
        anchors.right: parent.right;
        height: model.visible === 1 ? 30 : 0;

        visible: model.visible === 1;

//        MouseArea {
//            anchors.fill: parent;

//            onClicked: {
//                selectionBackGround.visible = true;
//            }
//        }

//        Rectangle {
//            id: selectionBackGround;
//            anchors.fill: parent;
//            color: "#4682B4";
//            opacity: 0.2;
//            radius: 2;
//            visible: false;
//        }

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
                // console.log("TreeItemDelegate CheckBox onCheckStateChanged", checkBox.checkState, model.packageId, model.Id);

//                 treeItemDelegate.checkBoxStateChanged(checkBox.checkState, model.packageId, model.Id);
                // listViewItem.changeCheckBoxState(checkBox.checkState, model.packageId, model.Id);
             }

             MouseArea {
                 anchors.fill: parent;
                 visible: model.isActive === 1;
                 onClicked: {
                     console.log("TreeItemDelegate CheckBox MouseArea onClicked", checkBox.checkState);
                     if (checkBox.checkState == 2) {
                         checkBox.checkState = 0
//                         model.stateChecked = 0;
                     }
                     else {
                         checkBox.checkState = 2
//                         model.stateChecked = 2;
                     }
                     listViewItem.changeCheckBoxState(checkBox.checkState, model.packageId, model.Id);
                 }
             }
        }

        Text {
            id: titleModel;
            anchors.left: iconArrow.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            text: model.Name;
            color: model.isActive === 1 ? Style.textColor : Style.disabledInActiveTextColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }
    }

    Column {
        id: childrenColumn;
        width: treeItemDelegate.width;
        anchors.top: mainRect.bottom;
        visible: treeItemDelegate.isOpened;

        Repeater {
             id: treeItemRepeater;
             delegate: Loader {
                 id: loader;
                 source: "TreeItemDelegate.qml";
                 onItemChanged: {
                     if (loader.item) {
                         loader.item.listViewItem = treeItemDelegate.listViewItem;
                     }
                 }
             }
       }
    }
}
