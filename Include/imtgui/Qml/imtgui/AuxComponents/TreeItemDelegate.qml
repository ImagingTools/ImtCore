import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    width: parent.width;

    height: childrenColumn.visible ? childrenColumn.height + treeItemDelegate.itemHeight : treeItemDelegate.itemHeight;

    property bool isOpened: model.isOpened;

    property int itemHeight: 30;
    property int index: model.index;

    Component.onCompleted: {
        console.log("TreeItemDelegate onCompleted");

        if (model.childItemModel){
            childModelRepeater.model = model.childItemModel;
        }
    }

    Rectangle {
        id: mainRect;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: parent.width;
        height: model.visible === 1 ? treeItemDelegate.itemHeight : 0;

        color: mainTreeView.currentParentIndex == model.index &&
               childModelRepeater.indexChild === -1 ? Style.selectedColor : Style.baseColor;

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
                    treeViewContainer.modelItems.SetData("isOpened", !treeItemDelegate.isOpened, model.index);
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

                property int indexChild: mainTreeView.currentChildIndex;

                delegate: Rectangle {
                    id: childRect;

                    width: treeItemDelegate.width;
                    height: childRect.visible ? treeItemDelegate.itemHeight : 0;

                    visible: model.visible === 1;

                    color: treeItemDelegate.index === mainTreeView.currentParentIndex &&
                           childModelRepeater.indexChild === model.index ? Style.selectedColor : "transparent";

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
                                treeViewContainer.checkBoxChanged(2 - checkBox.checkState, model.packageId, model.Id);
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
