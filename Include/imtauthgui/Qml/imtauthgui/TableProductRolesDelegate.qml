import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Item {
    id: productRolesDelegate;

    height: body.height + rolesColumn.height;

    property string commandsId;

    property int index: model.index;

    property int selectedIndex: -1;

    signal clicked(int index);
    signal doubleClicked(string id, string name);

    Component.onCompleted: {
        if (model.Roles){
            rolesRepeater.model = model.Roles;
        }
    }

    onSelectedIndexChanged: {
        if (productRolesDelegate.selectedIndex != productRolesDelegate.index){
            rolesRepeater.selectedIndex = -1;
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

    MouseArea {
        id: ma;

        anchors.fill: parent;

        onClicked: {
            productRolesDelegate.clicked(productRolesDelegate.index);
        }

        onDoubleClicked: {
            productRolesDelegate.doubleClicked();
        }
    }

    Rectangle {
        id: body;

        width: productRolesDelegate.width;
        height: 30;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.imagingToolsGradient2;

        Text {
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamilyBold;
            font.bold: true;

            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;

            text: model.Name;
        }

        AuxButton {
            id: buttonNew;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            width: 20;
            height: width;

            iconSource: "../../../../" + "Icons/" + Style.theme + "/Add_Off_Normal.svg";

            onClicked: {
                productRolesDelegate.clicked(productRolesDelegate.index);
                Events.sendEvent(productRolesDelegate.commandsId + "CommandActivated", "New");
            }
        }
    }

    Rectangle {
        id: rolesBg;

        anchors.top: body.bottom;

        color: "transparent";

        width: productRolesDelegate.width;
        height: rolesColumn.height;

        border.color: Style.imagingToolsGradient2;
        border.width: 1;

        Column {
            id: rolesColumn;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            width: rolesBg.width - 2;

            Repeater {
                id: rolesRepeater;

                property int selectedIndex: -1;

                onSelectedIndexChanged: {
                    if (productRolesDelegate.selectedIndex == productRolesDelegate.index){
                        baseCollectionView.selectedIndexChanged(rolesRepeater.selectedIndex);
                    }
                }

                delegate: Rectangle {
                    width: productRolesDelegate.width;
                    height: body.height;

                    color: rolesRepeater.selectedIndex == model.index ? Style.selectedColor : "transparent";

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;

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
                            rolesRepeater.selectedIndex = model.index;

                            productRolesDelegate.clicked(productRolesDelegate.index);
                        }

                        onDoubleClicked: {
                            productRolesDelegate.doubleClicked(model.Id, model.Name);
                        }
                    }
                }
            }
        }//Column
    }
}
