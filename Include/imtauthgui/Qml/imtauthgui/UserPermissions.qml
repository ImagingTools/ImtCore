import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userPermissionsContainer;

    property alias userPermissions: permissionsTable.elements;

    function updateGui(){
        console.log("UserPermissions updateGui");
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.alternateBaseColor;
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
                font.pixelSize: Style.fontSize_common;
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

                UserPermissionsController {
                    id: permissionsController;
                }

                AuxTable {
                    id: permissionsTable;

                    anchors.fill: parent;
                    anchors.margins: 2;

                    headers: headersModelPermissions;

                    delegate: TableUserPermissionsDelegate {
                        id: delegate;

                        width: parent.width;
                        height: 35;

                        selected: permissionsTable.selectedIndex === model.index;

                        onClicked: {
                            permissionsTable.selectedIndex = model.index;
                        }
                    }
                }//AuxTable permissionsTable
            }//Rectangle licensesBlock
        }//Column bodyColumn
    }//Flickable
}//Container
