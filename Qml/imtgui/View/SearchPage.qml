import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

ViewBase {
    id: root;

    model: TreeItemModel {}

    commandsControllerComp: Component {
        CommandsController {
            id: commandsController;

            isReady: true;

            Component.onCompleted: {
                commandsController.commandsModel = commandsModel2;
            }
        }
    }

    commandsDelegateComp: Component {
        ViewCommandsDelegateBase {
            onCommandActivated: {
                if (commandId == "Open"){
                    let indexes = table.getSelectedIndexes();
                    if (indexes.length > 0){
                        if (tabPanel.selectedIndex >= 0){
                            let typeId = root.model.getData("Id", tabPanel.selectedIndex);
                            let elements = table.elements;
                            let elementId = elements.getData("Id", indexes[0])

                            MainDocumentManager.openDocument(typeId, elementId);
                        }
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: commandsModel2;

        Component.onCompleted: {
            let index = commandsModel2.insertNewItem();
            commandsModel2.setData("Id", "Open", index);
            commandsModel2.setData("Name", "Open", index);
            commandsModel2.setData("Description", "", index);
            commandsModel2.setData("IsEnabled", false, index);
            commandsModel2.setData("Visible", true, index);
            commandsModel2.setData("Icon", "Icons/Edit", index);
            commandsModel2.setData("Status", "", index);

            if (root.commandsController){
                root.commandsController.commandsModel = commandsModel2;
            }
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("GlobalSearchActivated", root.update)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("GlobalSearchActivated", root.update)
    }

    function update(text){
        root.model.clear();
        table.elements = 0;
        tabPanel.selectedIndex = 0;
        gqlModel.send(text);
    }

    function updateGui(){
        if (tabPanel.selectedIndex >= 0){
            if (root.model.containsKey("Items", tabPanel.selectedIndex)){
                table.elements = root.model.getData("Items", tabPanel.selectedIndex);
            }
        }
    }

    TreeItemModel {
        id: headers;

        Component.onCompleted: {
            let index = headers.insertNewItem();
            headers.setData("Id", "Description", index)
            headers.setData("Name", "Description", index)

            table.headers = headers;
        }
    }

    TabPanel {
        id: tabPanel;
        anchors.top: parent.top;
        width: parent.width;
        model: root.model;
        displayRoleId: "Name";
        isCloseEnable: false;
        onSelectedIndexChanged: {
            table.resetSelection();
            root.doUpdateGui();
        }
    }

    Table {
        id: table;
        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        onSelectionChanged: {
            if (root.commandsController){
                root.commandsController.setCommandIsEnabled("Open", selection.length > 0);
            }
        }

        onDoubleClicked: {
            if (root.commandsDelegate){
                root.commandsDelegate.commandHandle("Open");
            }
        }
    }

    GqlModel {
        id: gqlModel;

        function send(text){
            var query = Gql.GqlRequest("query", "Search");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Text", text);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                console.log("Search onStateChanged", this.toJson());

                let resultModel;
                if (this.containsKey("data")){
                    resultModel = this.getData("data")
                }

                if (resultModel && resultModel.containsKey("Search")){
                    resultModel = resultModel.getData("Search")
                }

                if (resultModel){
                    root.model = resultModel;
                }

                root.doUpdateGui();
            }
        }
    }//GqlModel itemModel
}


