import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: permissionsProvider;

    property TreeItemModel model: TreeItemModel {};
    property TreeItemModel dependenciesModel: TreeItemModel {};

    property string productId: "";

    function updateModel(){
        if (permissionsProvider.productId == ""){
            console.error("Error::PermissionsProvider updateModel(): productId is empty!")
            return;
        }

        permissionModel.updateModel();
    }

    GqlModel {
        id: permissionModel;

        function updateModel() {
            console.log("updateModel ProductPermissions");
            var query = Gql.GqlRequest("query", "ProductPermissions");
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("ProductId", permissionsProvider.productId);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
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
                    if (dataModelLocal.ContainsKey("ProductPermissions")){
                        dataModelLocal = dataModelLocal.GetData("ProductPermissions");

                        if (dataModelLocal.ContainsKey("Features")){
                            permissionsProvider.model = dataModelLocal.GetData("Features");
                        }

                        if (dataModelLocal.ContainsKey("Dependencies")){
                            permissionsProvider.dependenciesModel = dataModelLocal.GetData("Dependencies");
                        }
                    }
                }
            }
        }
    }//GqlModel
}
