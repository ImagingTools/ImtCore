import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: permissionsProvider;

    property TreeItemModel permissionsModel: TreeItemModel {};

    function updateModel(){
        permissionsGqlModel.updateModel();
    }

    function clearModel(){
        permissionsModel.Clear();
    }

    GqlModel {
        id: permissionsGqlModel;

        function updateModel(){
            var query = Gql.GqlRequest("query", "GetPermissions") ;

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            query.AddField(queryFields);
            var gqlData = query.GetQuery();

            console.log("permissions log");
            this.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("State:",this.state, permissionsGqlModel)
            if (this.state == "Ready"){

                var dataModelLocal;

                if (this.ContainsKey("data")){
                    dataModelLocal = this.GetData("data");

                    if(dataModelLocal.ContainsKey("GetPermissions")){
                        dataModelLocal = dataModelLocal.GetData("PagesData");

                        //pagesProvider.pagesModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
