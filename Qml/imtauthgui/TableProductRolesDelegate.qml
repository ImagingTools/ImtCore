import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: productRolesDelegate;

    height: body.height + rolesColumn.height;

    property int index: model.index;

    property int selectedIndex: -1;

    property bool newIsEnabled: true;

    property int radius: 3;

    signal clicked(string id, int index);
    signal newClicked();
    signal doubleClicked(string id, int index);

    Component.onCompleted: {
        if (model.Roles){
            rolesRepeater.model = model.Roles;
        }
    }

    function getSelectedId(){
        let elements = model.Roles;

        let roleId = elements.GetData("Id", rolesRepeater.selectedIndex);

        return roleId;
    }

    function getSelectedName(){
        let elements = model.Roles;

        let roleName = elements.GetData("Name", rolesRepeater.selectedIndex);

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
//                Events.sendEvent(productRolesDelegate.commandsId + "CommandActivated", "New");
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

                    property bool selected: productRolesDelegate.selectedIndex == model.index;

                    color: this.selected ? Style.selectedColor : "transparent";

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
                            productRolesDelegate.selectedIndex = model.index;

                            productRolesDelegate.clicked(model.Id, model.index);
                        }

                        onDoubleClicked: {
                            productRolesDelegate.selectedIndex = model.index;

                            productRolesDelegate.doubleClicked(model.Id, model.index);
                        }
                    }
                }
            }
        }//Column
    }
}
