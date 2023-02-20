import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: delegateContainer;

    property var selectedIndex: delegateContainer.documentBase ? delegateContainer.documentBase.includedRolesTable.selectedIndex : 0;

    property TreeItemModel rolesModel: TreeItemModel {};

    property TreeItemModel documentsData: TreeItemModel {};

//    Component.onCompleted: {
//        console.log("RoleViewDelegate onCompleted", documentsData.GetData("ProductId", model.index));

//        delegateContainer.itemModelInputParams["ProductId"] = documentsData.GetData("ProductId", model.index);

//        delegateContainer.updateItemTimer = 100;
//        itemsModel.updateModel();
//    }

    onDocumentsDataChanged: {
        console.log("RoleViewDelegate onDocumentsDataChanged", documentsData.GetData("ProductId", model.index));

        delegateContainer.itemModelInputParams["ProductId"] = documentsData.GetData("ProductId", model.index);

//        delegateContainer.updateItemTimer = 100;
//        itemsModel.updateModel();
    }

    onSelectedIndexChanged: {
        console.log("RoleViewDelegate onSelectedIndexChanged", delegateContainer.selectedIndex);
        let isEnabled = delegateContainer.selectedIndex != null;

        delegateContainer.documentBase.commandsProvider.setCommandIsEnabled("Exclude", isEnabled);
    }

    onCommandActivated: {
        console.log("DocumentCommands onCommandActivated", delegateContainer.documentBase.itemId, commandId);
        if (commandId === "Include"){
            let productId = delegateContainer.documentBase.documentModel.GetData("ProductId");

            modalDialogManager.openDialog(rolesDialog, {"currentRoleId": delegateContainer.documentBase.itemId,
                                                        "productId": productId,
                                                        "model":     delegateContainer.rolesModel});
        }
        else if (commandId === "Exclude"){
            let indexes = delegateContainer.documentBase.includedRolesTable.selectedIndex.getIndexes();

            delegateContainer.documentBase.includedRolesTable.removeRow(indexes);

            delegateContainer.documentBase.includedRolesTable.selectedIndex = null;
        }
    }

    Component {
        id: rolesDialog;

        RolesDialog {
            onFinished: {
                if (buttonId == "Include"){

                    let parentId = this.selectedIndex.itemData.Id;
                    let parentName = this.selectedIndex.itemData.Name;

                    let row = {"Id": parentId, "Name": parentName};

                    for (let i = 0; i < delegateContainer.documentBase.includedRolesTable.rowCount; i++){
                        let rowObj = delegateContainer.documentBase.includedRolesTable.rowModel.get(i);
                        let rowId = rowObj["Id"];

                        if (parentId == rowId){
                            console.error("ERROR::The role cannot be included:", parentId);

                            return;
                        }
                    }

                    delegateContainer.documentBase.includedRolesTable.addRow(row);
                }
            }
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "RoleViewDelegate load items", "RolesList");
            var query = Gql.GqlRequest("query", "RolesList");

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields)

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                let dataModelLocal;

                if (itemsModel.ContainsKey("errors")){

                    return;
                }

                if (itemsModel.ContainsKey("data")){
                    dataModelLocal = itemsModel.GetData("data");

                    if (dataModelLocal.ContainsKey("RolesList")){
                        dataModelLocal = dataModelLocal.GetData("RolesList");
                        dataModelLocal = dataModelLocal.GetData("items");
                        delegateContainer.rolesModel = dataModelLocal;
                    }
                }
            }
        }
    }//GqlModel itemsModel
}
