import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: pagesProvider;

    property TreeItemModel pagesModel: TreeItemModel {};

    function updateModel(){
        pagesGqlModel.updateModel();
    }

    function clearModel(){
        pagesModel.Clear();
    }

    GqlModel {
        id: pagesGqlModel;

        function updateModel(){
            console.log("PagesProvider updateModel PagesData");
            var query = Gql.GqlRequest("query", "PagesData");

//            var inputParams = Gql.GqlObject("input");
//            inputParams.InsertField("ProductId", window.productId);
//            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("PageId");
            queryFields.InsertField("Name");
            queryFields.InsertField("Icon");
            queryFields.InsertField("Source");
            queryFields.InsertField("StartItem");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            console.log("gqlData", gqlData);

            this.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("State:",this.state, pagesGqlModel)
            if (this.state == "Ready"){

                if (this.ContainsKey("data")){
                    var dataModelLocal = this.GetData("data");

                    if(dataModelLocal.ContainsKey("PagesData")){
                        dataModelLocal = dataModelLocal.GetData("PagesData");

                        pagesProvider.pagesModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
