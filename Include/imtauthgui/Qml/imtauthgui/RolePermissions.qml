import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: rolePermissionsContainer;

    property int productIndex: -1;

    onProductIndexChanged: {
        container.productIndex = productIndex;
    }

    function updateGui(){
        console.log("RolePermissions updateGui");

        if (!documentModel.ContainsKey("Permissions")){
            documentModel.AddTreeModel("Permissions");
        }

        permissionsTable.elements = documentModel.GetData("Permissions");
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
                id: titlePermissions;

                text: qsTr("Permissions");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: permissionsBlock;

                anchors.horizontalCenter: bodyColumn.horizontalCenter;

                width: bodyColumn.width;
                height: 200;

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
