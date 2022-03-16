import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    width: 100;

    height: childrenColumn.visible ? 30 + childModelRepeater.count * 30 : 30;

    property bool isOpened: true;

    Component.onCompleted: {
        console.log("TreeItemDelegate onCompleted");

        console.log("model.visible", model.visible);

        if (model.childItemModel)
        {
            childModelRepeater.model = model.childItemModel;
        }
    }

    Rectangle {
        id: mainRect;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: parent.width;
        height: model.visible === 1 ? 30 : 0;

        color: Style.baseColor;

        visible: model.visible === 1;

        Image {
            id: iconArrow;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            width: 10;
            height: 10;

            visible: model.level === 0;
            source: treeItemDelegate.isOpened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                                "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

            sourceSize.height: height;
            sourceSize.width: width;

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;

                onClicked: {
                    console.log("TreeItemDelegate Image onClicked");
                    treeItemDelegate.isOpened = !treeItemDelegate.isOpened;
                }
            }
        }

        Text {
            id: parentTitle;

            anchors.left: iconArrow.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            text: model.Name;
            color: model.isActive === 1 ? Style.textColor : Style.disabledInActiveTextColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }

        Column {
            id: childrenColumn;

            anchors.top: mainRect.bottom;
            anchors.right: mainRect.right;

            width: treeItemDelegate.width - 20;

            visible: treeItemDelegate.isOpened;

            Repeater {
                 id: childModelRepeater;

                 onModelChanged: {
                     console.log("TreeItemDelegate onModelChanged", childModelRepeater.model.GetItemsCount());
                 }

                 delegate: Rectangle {
                     id: childRect;

                     width: treeItemDelegate.width;
                     height: 30;

                     CheckBox {
                          id: checkBox;

                          anchors.left: childRect.left;
                          anchors.leftMargin: 10;
                          anchors.verticalCenter: parent.verticalCenter;

                          checkState: model.stateChecked;

                          MouseArea {
                              anchors.fill: parent;

                              visible: model.isActive === 1;

                              onClicked: {
                                  console.log("TreeItemDelegate CheckBox MouseArea onClicked", checkBox.checkState);
                                  if (checkBox.checkState == 2) {
                                      checkBox.checkState = 0
                                  }
                                  else {
                                      checkBox.checkState = 2
                                  }

                                  mainTreeView.changeCheckBoxState(checkBox.checkState, model.packageId, model.Id);
                              }
                          }
                     }

                     Text {
                         id: childTitle;

                         anchors.left: checkBox.right;
                         anchors.leftMargin: 10;
                         anchors.verticalCenter: parent.verticalCenter;

                         text: model.Name;
                         color: model.isActive === 1 ? Style.textColor : Style.disabledInActiveTextColor;
                         font.pixelSize: Style.fontSize_common;
                         font.family: Style.fontFamily;
                     }
                 }
           }
        }
    }
}
