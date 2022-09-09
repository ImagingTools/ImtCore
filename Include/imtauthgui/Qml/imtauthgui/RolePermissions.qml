import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: rolePermissionsContainer;

    property alias rolePermissions: permissionsTable.elements;

    function updateGui(){
        console.log("RolePermissions updateGui");
    }

    Flickable {
        anchors.fill: parent;

        anchors.leftMargin: 20;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 500;

            spacing: 7;

            Text {
                id: titlePermissions;

                text: qsTr("Permissions");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_subtitle;
            }

            Text {
                id: titleProducts;

                text: qsTr("Products");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: products;

                width: parent.width;
                height: 23;

                radius: 3;

                onCurrentIndexChanged: {
                }
            }

            Rectangle {
                id: permissionsBlock;

                anchors.horizontalCenter: bodyColumn.horizontalCenter;

                width: bodyColumn.width;
                height: 300;

                color: Style.imagingToolsGradient1;

                border.width: 1;
                border.color: Style.borderColor;

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

                    anchors.fill: parent;
                    anchors.margins: 2;

                    headers: headersModelPermissions;

                    delegate: TableRolePermissionsDelegate {
                        id: delegate;

                        width: parent.width;
                        height: 35;

                        selected: permissionsTable.selectedIndex === model.index;

                        onClicked: {
                            permissionsTable.selectedIndex = model.index;
                        }
                    }
                }//AuxTable permissionsTable
            }//Rectangle permissionsBlock
        }//Column bodyColumn
    }//Flickable
}//Container
