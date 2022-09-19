import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: permissionsTableDelegate;

    color: Style.baseColor;

    property bool selected: false;
    property string name;

    signal clicked;
    signal doubleClicked;

    Rectangle{
        id: selectionBackGround;

        anchors.fill: parent;
        color: "#4682B4";

        opacity: 0.2;
        radius: 2;
        visible: permissionsTableDelegate.selected;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;

        onClicked: {
            permissionsTableDelegate.clicked();
        }

        onDoubleClicked: {
            permissionsTableDelegate.doubleClicked();
        }
    }

    Item {
        id: leftPart;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        width: parent.width;
        height: parent.height;

        clip: true;

        CheckBox {
            id: checkBoxPermission;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: leftPart.left;
            anchors.leftMargin: 10;

            checkState: model.State;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableRolePermissionsDelegate CheckBox onClicked");
                    model.State = 2 - checkBoxPermission.checkState;
                }
            }
        }

        Text {
            id: titlePermissionsTable;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxPermission.right;
            anchors.leftMargin: 10;

            text: model.Id;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;
        }
    }
}
