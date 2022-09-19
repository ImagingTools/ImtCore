import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    anchors.fill: parent;

    Component.onCompleted: {
        container.includedRolesTable = includesTable;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.alternateBaseColor;
    }

    function updateGui(){
        console.log("RoleEditor updateGui");
        roleIdInput.text = documentModel.GetData("Id");
        roleNameInput.text = documentModel.GetData("Name");

        productNameInput.text = documentModel.GetData("ProductId");

        let parents = documentModel.GetData("Parents");

        if (!parents){
            documentModel.AddTreeModel("Parents");
        }

        includesTable.elements = documentModel.GetData("Parents");
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
                id: titleProductId;

                text: qsTr("Product-ID");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: productNameInput;

                width: parent.width;
                height: 30;

                readOnly: true;
            }

            Text {
                id: titleRoleId;

                text: qsTr("Role-ID");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: roleIdInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Role-ID");

                onTextChanged: {
                    documentModel.SetData("Id", roleIdInput.text);
                }
            }

            Text {
                id: titleRoleName;

                text: qsTr("Role Name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: roleNameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Role name");

                onTextChanged: {
                    documentModel.SetData("Name", roleNameInput.text);
                }
            }

            Text {
                id: titleIncludes;

                text: qsTr("Included roles");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: tableBg;

                width: bodyColumn.width;
                height: 200;

                color: Style.imagingToolsGradient1;

                border.width: 1;
                border.color: Style.borderColor;

                TreeItemModel {
                    id: headersModelRoles;

                    Component.onCompleted: {
                        let index = headersModelRoles.InsertNewItem();
                        headersModelRoles.SetData("Id", "Name", index)
                        headersModelRoles.SetData("Name", "Name", index)
                    }
                }

                AuxTable {
                    id: includesTable;

                    anchors.fill: parent;
                    anchors.margins: 2;

                    headers: headersModelRoles;

                    onSelectedIndexChanged: {
                        console.log("includesTable onSelectedIndexChanged");

//                        container.selectedIndex = includesTable.selectedIndex;
                    }
                }//AuxTable includesTable
            }
        }//Column bodyColumn
    }//Flickable
}//Container
