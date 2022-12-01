import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: provider;

    property TreeItemModel model: TreeItemModel {};

    function updateModel(){
        dependenciesModel.updateModel();
    }

    GqlModel {
        id: dependenciesModel;

        function updateModel() {
            var query = Gql.GqlRequest("query", "FeaturesDependencies");

            var queryFields = Gql.GqlObject("dependencies");
            queryFields.InsertField("TreeModel");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, dependenciesModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");

                if (this.ContainsKey("data")){
                    dataModelLocal = this.GetData("data");

                    if (dataModelLocal.ContainsKey("FeaturesDependencies")){
                        dataModelLocal = dataModelLocal.GetData("FeaturesDependencies");
                    }
                    else{
                        dataModelLocal = dataModelLocal.AddTreeModel("FeaturesDependencies");
                    }

                    provider.model = dataModelLocal;
                }
            }
        }
    }
}
