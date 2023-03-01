import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: provider;

    property TreeItemModel model: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("PackagesCollectionUpdated", featuresModel.updateModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("PackagesCollectionUpdated", featuresModel.updateModel);
    }

    onModelChanged: {
        if (model){
            Events.sendEvent("FeaturesUpdated");
        }
    }

    function updateModel(){
        console.log( "provider updateModel");

        featuresModel.updateModel();
    }

    GqlModel {
        id: featuresModel;

        function updateModel() {
            console.log( "FeaturesTreeView GqlModel loadFeaturesModel");

            var query = Gql.GqlRequest("query", "FeaturesTree");

            var queryFields = Gql.GqlObject("treeItem");
            queryFields.InsertField("TreeModel");
            queryFields.InsertField("Successed");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("TreeView query ", gqlData);


            console.log("this", this);

//            Events.sendEvent("TreeViewModelUpdateStarted");
            featuresModel.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, featuresModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("FeaturesTree")){
                    dataModelLocal = dataModelLocal.GetData("FeaturesTree");
                    if (dataModelLocal.ContainsKey("TreeModel")) {
                        dataModelLocal = dataModelLocal.GetData("TreeModel");

                        provider.model = dataModelLocal;
                    }
                }
            }
        }
    }
}
