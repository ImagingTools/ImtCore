import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

QtObject {
    id: provider;

    property TreeItemModel model: TreeItemModel {}
    onModelChanged: {
        if (model){
            Events.sendEvent("FeaturesUpdated");
        }
    }

    function updateModel(){
        provider.featuresModel.updateModel();
    }

    function featureIsExists(featureId){
        for (let i = 0; i < provider.model.GetItemsCount(); i++){
            let packageId = provider.model.GetData("Id", i);
            let childModel = provider.model.GetData("ChildModel", i);
            if (childModel){
                let ok = provider.__featureExists(childModel, featureId);
                if (ok){
                    return true;
                }
            }
        }

        return false;
    }

    function __featureExists(externModel, featureId){
        for (let i = 0; i < externModel.GetItemsCount(); i++){
            let id = externModel.GetData("Id", i);
            if (id === featureId){
                return true;
            }

            let childModel = externModel.GetData("ChildModel", i);
            if (childModel){
                let ok = provider.__featureExists(childModel, featureId)
                if (ok){
                    return true;
                }
            }
        }

        return false;
    }

    property GqlModel featuresModel: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", "FeaturesTree");
            var queryFields = Gql.GqlObject("treeItem");
            queryFields.InsertField("TreeModel");
            queryFields.InsertField("Successed");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            provider.featuresModel.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, provider.featuresModel);
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
