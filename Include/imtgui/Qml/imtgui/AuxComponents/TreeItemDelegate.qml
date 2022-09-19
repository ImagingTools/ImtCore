import QtQuick 2.0
import Acf 1.0

Item {
    id: treeItemDelegate;

    height: childrenColumn.visible ? childrenColumn.height + body.height : body.height;

    property int itemHeight: 30;

    Component.onCompleted: {
        console.log("TreeItemDelegate onCompleted");

        if (model.ChildModel){
            childModelRepeater.model = model.ChildModel;
        }
    }

    Item {
        id: body;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: parent.width;
        height: model.Visible ? treeItemDelegate.itemHeight : 0;

        visible: model.Visible;

        Component.onCompleted: {
            if (model.ChildModel){
                iconArrow.visible = true;
            }
            else{
                checkBox.visible = true;
            }
        }

        Image {
            id: iconArrow;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            width: 10;
            height: 10;

            visible: false;

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

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            visible: false;

            checkState: model.State;

            onClicked: {
//                model.State = 2 - model.State;

                treeViewController.checkBoxChanged(2 - checkBox.checkState, model.Id);
            }
        }

        Text {
            id: title;

            anchors.left: iconArrow.right;
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
