import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userRolesContainer;

    property alias userRoles: rolesTable.elements;

    function updateGui(){
        console.log("UserRoles updateGui");

        let rolesModel = documentModel.GetData("Products");
        rolesTable.elements = rolesModel;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
        anchors.fill: parent;

        anchors.leftMargin: 20;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 400;

            spacing: 7;

            Text {
                id: titleRoles;

                text: qsTr("Roles");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            AuxTable {
                id: rolesTable;

                width: bodyColumn.width;
                height: 300;

                showHeaders: false;

                clip: true;

                delegate: TableUserRolesDelegate {
                    id: delegate;

                    width: parent.width;
                }

            }//AuxTable rolesTable

            Text {
                id: titlePermissions;

                text: qsTr("Permissions");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            TreeItemModel {
                id: headersModelPermissions;

                Component.onCompleted: {
                    let index = headersModelPermissions.InsertNewItem();
                    headersModelPermissions.SetData("Id", "Name", index)
                    headersModelPermissions.SetData("Name", "Name", index)
                }
            }

            AuxTable {
                id: permissionsTable;

                width: bodyColumn.width;
                height: 200;

                headers: headersModelPermissions;

                delegate: TableUserPermissionsDelegate {
                    width: parent.width;
                    height: 35;

                    selected: permissionsTable.selectedIndex === model.index;

                    onClicked: {
                        permissionsTable.selectedIndex = model.index;
                    }
                }
            }//AuxTable permissionsTable
        }//Column bodyColumn
    }//Flickable
}//Container
