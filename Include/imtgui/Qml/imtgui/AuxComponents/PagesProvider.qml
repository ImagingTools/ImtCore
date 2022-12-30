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
            var query = Gql.GqlRequest("query", "Get") ;

            var modelIdsFields = Gql.GqlObject("ModelIds");
            modelIdsFields.InsertField("PagesData");
            query.AddField(modelIdsFields);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("PageId");
            queryFields.InsertField("Name");
            queryFields.InsertField("Icon");
            queryFields.InsertField("Source");
            queryFields.InsertField("StartItem");
            query.AddField(queryFields);
            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("State:",this.state, pagesGqlModel)
            if (this.state == "Ready"){

                var dataModelLocal;

                if (this.ContainsKey("data")){
                    dataModelLocal = this.GetData("data");

                    if(dataModelLocal.ContainsKey("Get")){
                        dataModelLocal = dataModelLocal.GetData("Get");

                        pagesProvider.pagesModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
