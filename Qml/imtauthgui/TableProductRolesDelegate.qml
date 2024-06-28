import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

TableRowDelegateBase {
    id: productRolesDelegate;

    height: body.height + rolesColumn.height;

    property int index: model.index;

    property int selectedIndex: -1;

    property bool newIsEnabled: true;

    radius: 3;

    signal roleClicked(string id, int index);
    signal newClicked();
    signal roleDoubleClicked(string id, int index);

    Component.onCompleted: {
        if (model.Roles){
            rolesRepeater.model = model.Roles;
        }

        mouseArea.enabled = false;
    }

    function getSelectedId(){
        let elements = model.Roles;

        let roleId = elements.getData("Id", rolesRepeater.selectedIndex);

        return roleId;
    }

    function getSelectedName(){
        let elements = model.Roles;

        let roleName = elements.getData("Name", rolesRepeater.selectedIndex);

        return roleName;
    }

    Rectangle {
        id: body;

        width: productRolesDelegate.width;
        height: 30;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.imagingToolsGradient2;

        radius: productRolesDelegate.radius;

        Text {
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            width: parent.width;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamilyBold;
            font.bold: true;

            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;

            text: model.Name;
        }

        Button {
            id: buttonNew;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            width: 20;
            height: width;

            visible: productRolesDelegate.newIsEnabled;

            iconSource: "../../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
                productRolesDelegate.newClicked();
            }

            decorator: Component {
                id: buttonDecorator;

                ButtonDecorator {
                    color: buttonNew.hovered ? Style.buttonHoverColor : "transparent";
                    border.width: 0;
                }
            }
        }
    }

    Rectangle {
        id: rolesBg;

        anchors.top: body.bottom;

        color: Style.baseColor;

        width: body.width;
        height: rolesColumn.height + 2;

        border.color: Style.imagingToolsGradient2;
        border.width: 1;

        visible: rolesRepeater.count > 0;

        Column {
            id: rolesColumn;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            width: rolesBg.width - rolesBg.border.width * 2;

            Repeater {
                id: rolesRepeater;

                delegate: Rectangle {
                    width: rolesColumn.width;
                    height: body.height;

                    radius: productRolesDelegate.radius;

                    color: productRolesDelegate.selectedIndex === model.index ? Style.selectedColor : "transparent";

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Name;
                    }

                    MouseArea {
                        id: delegMa;

                        anchors.fill: parent;

                        onClicked: {
                            console.log("Roles MouseArea onClicked", model.index);

                            productRolesDelegate.selectedIndex = model.index;

                            productRolesDelegate.roleClicked(model.Id, model.index);
                        }

                        onDoubleClicked: {
                            console.log("RolesMouseArea onDoubleClicked");

                            productRolesDelegate.selectedIndex = model.index;

                            productRolesDelegate.roleDoubleClicked(model.Id, model.index);
                        }
                    }
                }
            }
        }//Column
    }
}
