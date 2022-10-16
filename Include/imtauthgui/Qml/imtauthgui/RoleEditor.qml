import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    anchors.fill: parent;

    signal commandModeChanged(string commandId, string newMode);

    Component.onCompleted: {
        container.includedRolesTable = includesTable;

        commandsProvider.modelLoaded.connect(onCommandsModelLoaded);
        commandsProvider.commandModeChanged.connect(commandModeChanged);
    }

    Component.onDestruction: {
        commandsProvider.modelLoaded.disconnect(onCommandsModelLoaded);
        commandsProvider.commandModeChanged.disconnect(commandModeChanged);
    }

    onCommandModeChanged: {
        for (let i = 0; i < commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsModel.GetData("Id", i);

            if (currentCommandId == commandId){
                commandsModel.SetData("Mode", newMode, i);
            }
        }
    }

    function onCommandsModelLoaded(){
        commandsProvider.mergeModelWith(commandsModel);
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    TreeItemModel {
        id: commandsModel;

        Component.onCompleted: {
            let index = commandsModel.InsertNewItem();

            commandsModel.SetData("Id", "Include", index);
            commandsModel.SetData("Name", "Include", index);
            commandsModel.SetData("Mode", "Normal", index);
            commandsModel.SetData("IconSource", "Add", index);
            commandsModel.SetData("Visible", false, index);

            index = commandsModel.InsertNewItem();

            commandsModel.SetData("Id", "Exclude", index);
            commandsModel.SetData("Name", "Exclude", index);
            commandsModel.SetData("Mode", "Disabled", index);
            commandsModel.SetData("IconSource", "Delete", index);
            commandsModel.SetData("Visible", false, index);

            repeater.model = commandsModel;
        }
    }

    function updateGui(){
        console.log("RoleEditor updateGui");
        roleIdInput.text = documentModel.GetData("Id");
        roleNameInput.text = documentModel.GetData("Name");
        descriptionInput.text = documentModel.GetData("Description");

        productId = documentModel.GetData("ProductId")

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

        clip: true;

        Column {
            id: bodyColumn;

            width: 400;

            spacing: 7;

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
                id: titleDescription;

                text: qsTr("Description");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: descriptionInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the description");

                onTextChanged: {
                    documentModel.SetData("Description", descriptionInput.text);
                }
            }

            Text {
                id: titleIncludes;

                text: qsTr("Included roles");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Item {
                id: rowCommands;

                height: 250;
                width: bodyColumn.width;

                clip: true;

                MouseArea {
                    anchors.fill: parent;
                }

                Rectangle {
                    id: topRect;

                    width: parent.width;
                    height: 25;

                    color: Style.alternateBaseColor;

                    Row {
                        id: row;

                        anchors.horizontalCenter: parent.horizontalCenter;
                        anchors.verticalCenter: parent.verticalCenter;

                        spacing: 10;

                        Repeater {
                            id: repeater;

                            delegate: AuxButton {
                                anchors.verticalCenter: rowCommands.verticalCenter;

                                width: 18;
                                height: width;

                                iconSource: "../../../../Icons/Light/" + model.IconSource +"_Off_" + model.Mode + ".svg";

                                enabled: model.Mode == "Normal";

                                onClicked: {
                                    Events.sendEvent(commandsId + "CommandActivated", model.Id);
                                }
                            }
                        }
                    }
                }

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

                    anchors.top: topRect.bottom;

                    width: parent.width;
                    height: 200;

                    headers: headersModelRoles;
                }//AuxTable includesTable
            }
        }//Column bodyColumn
    }//Flickable
}//Container
