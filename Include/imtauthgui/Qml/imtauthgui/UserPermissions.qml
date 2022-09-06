import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

DocumentBase {
    id: userPermissionsContainer;

    property alias userPermissions: permissionsTable.elements;

    commandsDelegatePath: "../../imtlicgui/UserEditorCommandsDelegate.qml"

    onDocumentModelChanged: {

        let userPermissionsModel = documentModel.GetData("Permissions");
        if (!userPermissionsModel){
            userPermissionsModel = documentModel.AddTreeModel("Permissions");
        }

        permissionsTable.elements = userPermissionsModel;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: userPermissionsContainer.commandsId;
        editorItem: userPermissionsContainer;

        onModelParsed: {
            userPermissions = documentModel.GetData("Permissions");
            updateGui();
        }
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
                        headersModelPermissions.SetData("PermissionId", "PermissionId", index)
                    }
                }

                UserPermissionsController {
                    id: permissionsController;
                }

                AuxTable {
                    id: permissionsTable;

                    anchors.fill: parent;
                    anchors.margins: 10;

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
