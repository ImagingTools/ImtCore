import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    Component.onCompleted: {
        console.log("RoleViewDelegate onCompleted");

        updateItemTimer = 100;
        itemsModel.updateModel();
    }

    onSelectedIndexChanged: {
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
    }

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "RoleViewDelegate load items", "ProductsList");
            var query = Gql.GqlRequest("query", "ProductsList");

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields)

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditor items query  ", gqlData);

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

                    if (dataModelLocal.ContainsKey("ProductsList")){
                        dataModelLocal = dataModelLocal.GetData("ProductsList");
                        dataModelLocal = dataModelLocal.GetData("items");

                        products.model = dataModelLocal;
                    }
                }
            }
        }
    }//GqlModel itemsModel
}

