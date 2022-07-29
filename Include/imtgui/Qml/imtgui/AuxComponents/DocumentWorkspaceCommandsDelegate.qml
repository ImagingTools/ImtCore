import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

Item {
    id: container;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    property Item objectView;
    property Item tableData;
    property Item commandsProvider;

    property int selectedIndex: container.tableData.selectedIndex;

    property alias editorDialog: editDialog;

    onSelectedIndexChanged: {
        console.log("CollectionViewCommands onSelectedIndexChanged", container.selectedIndex, container);
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";

        container.commandsProvider.changeCommandMode("Remove", mode);
        container.commandsProvider.changeCommandMode("Edit", mode);
    }

    Component {
        id: messageDialog;
        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    let elements = container.tableData.elements;
                    let selectedIndex = container.tableData.selectedIndex;
                    elements.RemoveItem(selectedIndex);
                }
            }
        }
    }

    Component {
        id: editDialog;
        EditDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    let elementsModel = container.tableData.elements;
                    elementsModel.SetData("Id", valueId, container.selectedIndex);
                    elementsModel.SetData("Name", valueName, container.selectedIndex);

                    objectView.updateGui();
                }
            }
        }
    }

    function commandHandle(commandId){
        console.log("PackageCommands commandActivated", commandId);

        if (commandId === "New"){
            let elements = container.tableData.elements;
            let index = elements.InsertNewItem();
            elements.SetData("Name", "Item Name", index);
            elements.SetData("Id", "", index);
            elements.SetData("Description", "", index);
        }
        else if (commandId === "Remove"){
            modalDialogManager.openDialog(messageDialog, {"message": qsTr("Remove selected item from the document ?")});
        }
        else if (commandId === "Edit"){
            let id = container.tableData.getSelectedId();
            let name = container.tableData.getSelectedName();
            modalDialogManager.openDialog(editorDialog, {"titleId": qsTr("Item-ID"),
                                                         "titleName": qsTr("Item Name"),
                                                         "valueId": id,
                                                         "valueName": name});
        }
        else if (commandId === "Save"){
            let itemId = container.objectView.itemId;
            if (itemId === ""){
                container.gqlModelQueryType = "Add";
                container.gqlModelQueryTypeNotify = "addedNotification";
            }
            else{
                container.gqlModelQueryType = "Update";
                container.gqlModelQueryTypeNotify = "updatedNotification";
            }

            saveQuery.updateModel();
        }
        else if (commandId === "Close"){
            multiDocView.closeTab(tabPanelInternal.selectedIndex);
        }
    }

    TreeItemModel {
        id: model;
    }

    GqlModel {
        id: saveQuery;

        function updateModel(){
            console.log( "updateModel saveModel");
            var query = Gql.GqlRequest("query", container.commandsId + container.gqlModelQueryType);
            var queryFields = Gql.GqlObject(container.gqlModelQueryTypeNotify);

            let itemId = container.objectView.itemId;
            let itemName = container.objectView.itemName;

            console.log("itemId", itemId);
            console.log("itemName", itemName);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);

            model.SetData("Id", itemId);
            model.SetData("Name", itemName);
            model.SetData("Items", container.tableData.elements);

            var jsonString = model.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

            query.AddParam(inputParams);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ObjectView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveQuery);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (saveQuery.ContainsKey("errors")){
                    return;
                }

                if (saveQuery.ContainsKey("data")){
                    dataModelLocal = saveQuery.GetData("data");

                    dataModelLocal = dataModelLocal.GetData(container.commandsId + "Add");

                    if (dataModelLocal.ContainsKey("addedNotification")){
                        dataModelLocal = dataModelLocal.GetData("addedNotification");

                    }
                }

                Events.sendEvent("PackageCollectionUpdateGui");
            }
        }
    }
}

