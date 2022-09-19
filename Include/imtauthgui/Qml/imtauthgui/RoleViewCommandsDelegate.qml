import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: delegateContainer;

    property int selectedIndex: includedRolesTable.selectedIndex;

    property int pageIndex: container.pageIndex;

    Component.onCompleted: {
        console.log("RoleViewDelegate onCompleted");

        itemModelInputParams["ProductId"] = documentsData.GetData("ProductId", model.index);

        updateItemTimer = 100;
        itemsModel.updateModel();
    }

    onSelectedIndexChanged: {
        console.log("RoleViewDelegate onSelectedIndexChanged", selectedIndex);
        let mode = delegateContainer.selectedIndex > -1 ? "Normal" : "Disabled";

        commandsProvider.changeCommandMode("Exclude", mode);
    }

    onPageIndexChanged: {
        let mode = pageIndex == 0 ? "Normal" : "Disabled";

        commandsProvider.changeCommandMode("Include", mode);

        if (pageIndex != 0){
            commandsProvider.changeCommandMode("Exclude", "Disabled")
        }
        else{
            delegateContainer.selectedIndexChanged();
        }
    }

    onCommandActivated: {
        console.log("DocumentCommands onCommandActivated", container.itemId, commandId);

        if (commandId === "Include"){
            let productId = documentModel.GetData("ProductId");

            modalDialogManager.openDialog(rolesDialog, {"productId": productId,
                                                        "model":     rolesModel});
        }
        else if (commandId === "Exclude"){
            let includesRoles = documentModel.GetData("Parents");
            includesRoles.RemoveItem(selectedIndex);

            let countRoles = includesRoles.GetItemsCount();
            if (selectedIndex >= countRoles){
                includedRolesTable.selectedIndex = -1;
            }
        }
    }

    Component {
        id: rolesDialog;

        RolesDialog {
            onFinished: {
                if (buttonId == "Include"){
                    let roleId = documentModel.GetData("Id");
                    for (let i = 0; i < rolesModel.GetItemsCount(); i++){
                        let currentProductId = rolesModel.GetData("Id", i);

                        if (currentProductId == productId){
                            let roles = rolesModel.GetData("Roles", i);

                            let selectedRoleId = roles.GetData("Id", selectedIndex);
                            let selectedRoleName = roles.GetData("Name", selectedIndex);

                            if (selectedRoleId == roleId){
                                continue;
                            }

                            let includesRoles = documentModel.GetData("Parents");
                            let roleFind = false;
                            for (let j = 0; j < includesRoles.GetItemsCount(); j++){
                                let roleId = includesRoles.GetData("Id", j);

                                if (roleId == selectedRoleId){
                                    roleFind = true;
                                    break;
                                }
                            }

                            if (!roleFind){
                                let index = includesRoles.InsertNewItem();

                                includesRoles.SetData("Id", selectedRoleId, index);
                                includesRoles.SetData("Name", selectedRoleName, index);
                            }
                        }
                    }
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
                        rolesModel = dataModelLocal;
                    }
                }
            }
        }
    }//GqlModel itemsModel

    GqlModel {
        id: permissionModel;

        function updateModel(productId) {
            var query = Gql.GqlRequest("query", "ProductFeatures");
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("ProductId", productId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("style");
            queryFields.InsertField("theme");
            queryFields.InsertField("source");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("permissionModel gqlData", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, permissionModel);
            if (this.state === "Ready"){
                let dataModelLocal;

                if (permissionModel.ContainsKey("errors")){

                    return;
                }

                if (permissionModel.ContainsKey("data")){
                    dataModelLocal = permissionModel.GetData("data");
                    dataModelLocal = dataModelLocal.GetData("ProductFeatures");

                    documentModel.SetData("Permissions", dataModelLocal);

                    container.updateGui();
                }
            }
        }
    }//GqlModel itemsModel
}
