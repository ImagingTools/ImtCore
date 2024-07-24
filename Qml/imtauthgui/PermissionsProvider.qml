import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0

Item {
    id: permissionsProvider;

    property TreeItemModel dataModel: TreeItemModel {};
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
            var query = Gql.GqlRequest("query", "ProductPermissions");
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("ProductId", permissionsProvider.productId);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                let dataModelLocal;

                if (permissionModel.containsKey("errors")){
                    return;
                }

                if (permissionModel.containsKey("data")){
                    dataModelLocal = permissionModel.getData("data");
                    if (dataModelLocal.containsKey("ProductPermissions")){
                        dataModelLocal = dataModelLocal.getData("ProductPermissions");
                        permissionsProvider.dataModel = dataModelLocal;

//                        if (dataModelLocal.containsKey("Dependencies")){
//                            permissionsProvider.dependenciesModel = dataModelLocal.getData("Dependencies");
//                        }
                    }
                }
            }
        }
    }//GqlModel
}
