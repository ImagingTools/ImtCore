import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    signal commandModeChanged(string commandId, string newMode);

    Component.onCompleted: {
        container.includedRolesTable = includesTable;

        commandsProvider.modelLoaded.connect(onCommandsModelLoaded);
        commandsProvider.commandModeChanged.connect(commandModeChanged);

        roleNameInput.focus = true;
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

    onDocumentModelChanged: {
        updateGui();
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
        console.log("RoleEditor updateGui", documentModel.toJSON());

        blockUpdatingModel = true;

        if (documentModel.ContainsKey("Id")){
            roleIdInput.text = documentModel.GetData("Id");
        }

        if (documentModel.ContainsKey("Name")){
            roleNameInput.text = documentModel.GetData("Name");
        }

        if (documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }

        let parents = documentModel.GetData("Parents");
        if (!parents){
            parents = documentModel.AddTreeModel("Parents");
        }

        includesTable.rowModel.clear();
        includesTable.height = includesTable.headerHeight + includesTable.rowItemHeight;

        for (let i = 0; i < parents.GetItemsCount(); i++){
            let parentId = parents.GetData("Id", i);
            let parentName = parents.GetData("Name", i);

            let row = {"Id": parentId, "Name": parentName}

            includesTable.addRow(row);
        }

        blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("RoleEditor updateModel");
        undoRedoManager.beginChanges();

        documentModel.SetData("Id", roleIdInput.text);
        documentModel.SetData("Name", roleNameInput.text);
        documentModel.SetData("Description", descriptionInput.text);

        let parents = documentModel.AddTreeModel("Parents");

        let rowModel = includesTable.rowModel;

        for (let i = 0; i < includesTable.rowModel.count; i++){
            let rowObj = includesTable.rowModel.get(i);

            let index = parents.InsertNewItem();

            parents.SetData("Id", rowObj["Id"], index);
            parents.SetData("Name", rowObj["Name"], index);
        }

        undoRedoManager.endChanges();
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

            height: roleEditorContainer.height;
            width: 400;

            spacing: 7;

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

                onEditingFinished: {
                    console.log("roleNameInput onEditingFinished");
                    let oldText = documentModel.GetData("Name");
                    if (oldText != roleNameInput.text){
                        roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
                        updateModel();
                    }
                }

                KeyNavigation.tab: roleIdInput;
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
                readOnly: true;

                KeyNavigation.tab: descriptionInput;
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

                onEditingFinished: {
                    let oldText = documentModel.GetData("Description");
                    if (oldText != descriptionInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: roleNameInput;
            }

            Text {
                id: titleIncludes;

                text: qsTr("Included roles");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Item {
                width: parent.width;
                height: 325;

                Rectangle {
                    id: commands;

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

                BasicTableView {
                    id: includesTable;

                    anchors.top: commands.bottom;

                    width: parent.width;
                    height: headerHeight + rowItemHeight;

                    rowDelegate: TableViewItemDelegateBase {
                        root: includesTable;

                        Component.onCompleted: {
                            let newHeight = includesTable.rowCount * includesTable.rowItemHeight + includesTable.headerHeight;
                            if (newHeight > includesTable.height){
                                includesTable.height = newHeight;
                            }
                        }

                        Component.onDestruction: {
                            let newHeight = includesTable.height - height;
                            if (newHeight >= includesTable.headerHeight + includesTable.rowItemHeight){
                                includesTable.height = newHeight;
                            }
                        }
                    }

                    Component.onCompleted: {
                        includesTable.addColumn({"Id": "Name", "Name": "Name"})
                    }

                    onRowAdded: {
                        if (!blockUpdatingModel){
                            updateModel();
                        }
                    }

                    onRowRemoved: {
                        if (!blockUpdatingModel){
                            updateModel();
                        }
                    }
                }
            }
        }//Column bodyColumn
    }//Flickable
}//Container
