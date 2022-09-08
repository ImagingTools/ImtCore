import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userRolesContainer;

    property alias userRoles: rolesTable.elements;

    function updateGui(){
        console.log("UserRoles updateGui");

        let rolesModel = documentModel.GetData("Roles");
        rolesTable.elements = rolesModel;
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
                id: titleRoles;

                text: qsTr("Roles");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: rolesBlock;

                anchors.horizontalCenter: bodyColumn.horizontalCenter;

                width: bodyColumn.width;
                height: 300;

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

                UserRolesController {
                    id: licensesController;
                }

                AuxTable {
                    id: rolesTable;

                    anchors.fill: parent;
                    anchors.margins: 10;

                    headers: headersModelRoles;

                    delegate: TableUserRolesDelegate {
                        id: delegate;

                        width: parent.width;
                        height: 35;

                        selected: rolesTable.selectedIndex === model.index;

                        onClicked: {
                            rolesTable.selectedIndex = model.index;
                        }
                    }
                }//AuxTable rolesTable
            }//Rectangle licensesBlock
        }//Column bodyColumn
    }//Flickable
}//Container
