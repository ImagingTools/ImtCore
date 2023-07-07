import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: pagesProvider;

    property TreeItemModel pagesModel: TreeItemModel {};

    function updateModel(){
        pagesProvider.pagesGqlModel.updateModel();
    }

    function clearModel(){
        pagesModel.Clear();
    }

    property string modelState: pagesGqlModel.state;

    property GqlModel pagesGqlModel : GqlModel {
        function updateModel(){
            var query = Gql.GqlRequest("query", "PagesData");

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
            if (this.state === "Ready"){
                if (this.ContainsKey("data")){
                    var dataModelLocal = this.GetData("data");

                    if(dataModelLocal.ContainsKey("PagesData")){
                        dataModelLocal = dataModelLocal.GetData("PagesData");

                        console.log("dataModelLocal1", dataModelLocal)
                        console.log("dataModelLocal2", dataModelLocal.toJSON())

                        pagesProvider.pagesModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
