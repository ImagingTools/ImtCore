import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;
    width: 100;
    height: childrenColumn.visible ? mainRect.height + childrenColumn.height: mainRect.height;
    property bool isOpened: false;

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

//        Rectangle {
//            id: buttonClick;
//            width: 10;
//            height: width;
//            //visible: treeItemRepeater.count > 0;
//            anchors.leftMargin: 10;
//            anchors.left: checkBox.right;
//            anchors.verticalCenter: parent.verticalCenter;
//            color: "transparent";
//            Text {
//                id: znak;
//                text: treeItemDelegate.isOpened ? "-" : "+";
//                anchors.horizontalCenter: parent.horizontalCenter;
//                anchors.verticalCenter: parent.verticalCenter;
//                color: Style.textColor;
//            }

//            MouseArea {
//                id: mouseArea;
//                anchors.fill: parent;
//                onClicked: {
//                    treeItemDelegate.isOpened = !treeItemDelegate.isOpened;
//                }
//            }
//        }

        Image {
            id: iconArrow;
            width: 10;
            height: 10;
            anchors.left: parent.left;
            anchors.leftMargin: 10;
//            anchors.left: checkBox.right;
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
