import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: rolesProvider;

    property TreeItemModel model: TreeItemModel {};

    function updateModel(){
        rolesModel.updateModel();
    }

    GqlModel {
        id: rolesModel;

        function updateModel() {
            var query = Gql.GqlRequest("query", "RolesList");

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("gqlModelBaseContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, rolesModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (rolesModel.ContainsKey("errors")){
                    return;
                }

                if (rolesModel.ContainsKey("data")){
                    dataModelLocal = rolesModel.GetData("data");
                    if (dataModelLocal.ContainsKey("RolesList")){
                        dataModelLocal = dataModelLocal.GetData("RolesList");
                        if (dataModelLocal.ContainsKey("items")){
                            rolesProvider.model = dataModelLocal.GetData("items");
                        }
                    }
                }
            }
        }
    }
}
