import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0

QtObject {
    id: permissionsProvider;

    property string productId: "";
    property TreeItemModel permissionsModel: TreeItemModel {};

    function updateModel(){
        if (permissionsProvider.productId == ""){
            console.error("Unable to update model for permissions. Error: Product-ID is empty")
            return;
        }

        permissionModel.send();
    }

    function getHeaders(){
        return {};
    }

    property GqlRequestSender permissionModel: GqlRequestSender {
        requestType: 0; // Query
        gqlCommandId: "ProductPermissions";

        function createQueryParams(query){
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("ProductId", permissionsProvider.productId);
            query.AddParam(inputParams);
        }

        function onResult(data){
            console.log("PermissionsProvider.qmlonResult", data.toJson());
            permissionsProvider.permissionsModel = data;
        }

        function getHeaders(){
            return permissionsProvider.getHeaders();
        }
    }//GqlModel
}
