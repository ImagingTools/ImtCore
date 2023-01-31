import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    property int mainMargin: 0;
    property int panelWidth: 400;

    signal commandModeChanged(string commandId, bool newMode);

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
                commandsModel.SetData("IsEnabled", newMode, i);
            }
        }
    }

    onDocumentModelChanged: {
        roleEditorContainer.updateGui();
    }

    function onCommandsModelLoaded(){
        commandsProvider.mergeModelWith(commandsModel);
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.backgroundColor;
        Loader{
            id: backgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(backgroundDecoratorLoader.item){
                    backgroundDecoratorLoader.item.rootItem = background;
                }
            }
        }
    }

    TreeItemModel {
        id: commandsModel;

        Component.onCompleted: {
            let index = commandsModel.InsertNewItem();

            commandsModel.SetData("Id", "Include", index);
            commandsModel.SetData("Name", "Include", index);
            commandsModel.SetData("IsEnabled", true, index);
            commandsModel.SetData("Icon", "Add", index);
            commandsModel.SetData("Visible", false, index);

            index = commandsModel.InsertNewItem();

            commandsModel.SetData("Id", "Exclude", index);
            commandsModel.SetData("Name", "Exclude", index);
            commandsModel.SetData("IsEnabled", false, index);
            commandsModel.SetData("Icon", "Delete", index);
            commandsModel.SetData("Visible", false, index);

            repeater.model = commandsModel;
        }
    }

    function updateGui(){
        console.log("RoleEditor updateGui", documentModel.toJSON());

        roleEditorContainer.blockUpdatingModel = true;

        if (roleEditorContainer.documentModel.ContainsKey("Id")){
            roleIdInput.text = documentModel.GetData("Id");
        }

        if (roleEditorContainer.documentModel.ContainsKey("Name")){
            roleNameInput.text = documentModel.GetData("Name");
        }

        if (roleEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }

        includesTable.rowModel.clear();
        includesTable.height = includesTable.headerHeight + includesTable.rowItemHeight;

        if (documentModel.ContainsKey("Parents")){
            let parents = documentModel.GetData("Parents");

            for (let i = 0; i < parents.GetItemsCount(); i++){
                let parentId = parents.GetData("Id", i);
                let parentName = parents.GetData("Name", i);

                let row = {"Id": parentId, "Name": parentName}

                includesTable.addRow(row);
            }
        }

        roleEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("RoleEditor updateModel");
        roleEditorContainer.undoRedoManager.beginChanges();

        roleEditorContainer.documentModel.SetData("Id", roleIdInput.text);
        roleEditorContainer.documentModel.SetData("Name", roleNameInput.text);
        roleEditorContainer.documentModel.SetData("Description", descriptionInput.text);

        let parents = roleEditorContainer.documentModel.AddTreeModel("Parents");

        let rowModel = includesTable.rowModel;

        for (let i = 0; i < includesTable.rowModel.count; i++){
            let rowObj = includesTable.rowModel.get(i);

            let index = parents.InsertNewItem();

            parents.SetData("Id", rowObj["Id"], index);
            parents.SetData("Name", rowObj["Name"], index);
        }

        roleEditorContainer.undoRedoManager.endChanges();
    }

    Flickable {
        anchors.fill: parent;
        anchors.leftMargin: 0;

        contentWidth: columnContainer.width + columnContainer.anchors.leftMargin;
        contentHeight: columnContainer.height + columnContainer.anchors.topMargin;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;


        Item{
            id: columnContainer;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.leftMargin: 2;
            anchors.topMargin: 2;

            width: roleEditorContainer.panelWidth;
            height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;
            Loader{
                id: mainPanelFrameLoader;

                anchors.fill: parent;

                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;

                onLoaded: {
                    if(mainPanelFrameLoader.item){
                        roleEditorContainer.mainMargin = mainPanelFrameLoader.item.mainMargin;
                    }
                }
            }
            Column {
                id: bodyColumn;

                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.topMargin: roleEditorContainer.mainMargin;
                anchors.leftMargin: roleEditorContainer.mainMargin;

                width: roleEditorContainer.panelWidth - 2*anchors.leftMargin;

                spacing: 10;


                Text {
                    id: titleRoleName;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Role Name");

                    Loader{
                        id: titleDecoratorLoader1;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader1.item){
                                titleDecoratorLoader1.item.rootItem = titleRoleName;
                            }
                        }
                    }
                }

                CustomTextField {
                    id: roleNameInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the role name");

                    onEditingFinished: {
                        console.log("roleNameInput onEditingFinished");
                        let oldText = roleEditorContainer.documentModel.GetData("Name");
                        if (oldText != roleNameInput.text){
                            roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
                            roleEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: roleIdInput;

                    Loader{
                        id: inputDecoratorLoader1;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader1.item){
                                inputDecoratorLoader1.item.rootItem = roleNameInput;
                            }
                        }
                    }
                }

                Text {
                    id: titleRoleId;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Role-ID");

                    Loader{
                        id: titleDecoratorLoader2;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader2.item){
                                titleDecoratorLoader2.item.rootItem = titleRoleId;
                            }
                        }
                    }
                }

                CustomTextField {
                    id: roleIdInput;

                    width: parent.width;
                    height: 30;
                    readOnly: true;

                    KeyNavigation.tab: descriptionInput;

                    Loader{
                        id: inputDecoratorLoader2;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader2.item){
                                inputDecoratorLoader2.item.rootItem = roleIdInput;
                            }
                        }
                    }
                }

                Text {
                    id: titleDescription;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Description");

                    Loader{
                        id: titleDecoratorLoader3;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader3.item){
                                titleDecoratorLoader3.item.rootItem = titleDescription;
                            }
                        }
                    }
                }

                CustomTextField {
                    id: descriptionInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the description");

                    onEditingFinished: {
                        let oldText = roleEditorContainer.documentModel.GetData("Description");
                        if (oldText != descriptionInput.text){
                            roleEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: roleNameInput;

                    Loader{
                        id: inputDecoratorLoader3;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader3.item){
                                inputDecoratorLoader3.item.rootItem = descriptionInput;
                            }
                        }
                    }
                }

                Text {
                    id: titleIncludes;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Included roles");

                    Loader{
                        id: titleDecoratorLoader4;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader4.item){
                                titleDecoratorLoader4.item.rootItem = titleIncludes;
                            }
                        }
                    }
                }

                Item {
                    width: parent.width;
                    height: commands.height + includesTable.height + includesTable.anchors.topMargin;
                    visible: true;

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

                                    iconSource: enabled ? "../../../../Icons/Light/" + model.Icon +"_Off_Normal.svg" :
                                                                  "../../../../Icons/Light/" + model.Icon +"_Off_Disabled.svg";

                                    enabled: model.IsEnabled;

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
                        anchors.topMargin: 0;

                        width: parent.width;
                        height: headerHeight + rowItemHeight * includesTable.tableListView.count;

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
                            if (!roleEditorContainer.blockUpdatingModel){
                                roleEditorContainer.updateModel();
                            }
                        }

                        onRowRemoved: {
                            if (!roleEditorContainer.blockUpdatingModel){
                                roleEditorContainer.updateModel();
                            }
                        }
                    }
                }
            }//Column bodyColumn
        }
    }//Flickable
}//Container
