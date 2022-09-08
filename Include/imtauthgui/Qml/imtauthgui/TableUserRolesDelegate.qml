import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: rolesTableDelegate;

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
        visible: rolesTableDelegate.selected;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;

        onClicked: {
            rolesTableDelegate.clicked();
        }

        onDoubleClicked: {
            rolesTableDelegate.doubleClicked();
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
            id: checkBoxRole;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: leftPart.left;
            anchors.leftMargin: 10;

            checkState: model.State;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableUserRolesDelegate CheckBox onClicked");
                    model.State = 2 - checkBoxRole.checkState;
                }
            }
        }

        Text {
            id: titleRolesTable;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxRole.right;
            anchors.leftMargin: 10;

            text: model.Name;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;
        }
    }
}
