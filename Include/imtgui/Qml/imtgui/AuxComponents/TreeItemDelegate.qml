import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    height: childrenColumn.visible ? childrenColumn.height + body.height : body.height;

    property int itemHeight: 30;

    property TreeItemModel childModel: model.ChildModel ? model.ChildModel : null;

    signal doubleClicked;

    onChildModelChanged: {
        console.log("onChildModelChanged");
        childModelRepeater.model = model.ChildModel;
    }

    Rectangle {
        id: body;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: parent.width;
        height: model.Visible ? treeItemDelegate.itemHeight : 0;

        visible: model.Visible;

        color: model.Selected ? Style.selectedColor : "transparent";

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                resetSelectedItem(modelItems);
                model.Selected = true;
            }

            onDoubleClicked: {
                treeItemDelegate.doubleClicked();
            }
        }

        Image {
            id: iconArrow;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            width: 10;
            height: 10;

            visible: childModelRepeater.count > 0;

            source: model.Opened ? "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg" :
                                     "../../../" + "Icons/" + Style.theme + "/" + "Right" + "_On_Normal.svg";

            sourceSize.height: height;
            sourceSize.width: width;

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;

                onClicked: {
                    console.log("iconArrow onClicked");
                    model.Opened = !model.Opened;
                }
            }
        }

        CheckBox {
            id: checkBox;

            anchors.left: iconArrow.right;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            checkState: model.State;

            onClicked: {
//                model.State = 2 - model.State;

                treeViewController.checkBoxChanged(2 - checkBox.checkState, model.Id);
            }
        }

        Text {
            id: title;

            anchors.left: checkBox.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            text: model.Name;
            color: model.Active ? Style.textColor : Style.disabledInActiveTextColor;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }
    }

    Column {
        id: childrenColumn;

        anchors.top: body.bottom;
        anchors.right: parent.right;

        width: treeItemDelegate.width - 20;

        visible: model.Opened;

        Repeater {
            id: childModelRepeater;

            delegate: delegateComp;
        }
    }
}
