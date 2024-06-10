pragma Singleton

import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: provider;

    property TreeItemModel model: TreeItemModel {}

    function updateModel(){
        provider.featuresModel.updateModel();
    }

    function featureIsExists(featureId){
        for (let i = 0; i < provider.model.getItemsCount(); i++){
            let packageId = provider.model.getData("Id", i);
            let childModel = provider.model.getData("ChildModel", i);
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
        for (let i = 0; i < externModel.getItemsCount(); i++){
            let id = externModel.getData("Id", i);
            if (id === featureId){
                return true;
            }

            let childModel = externModel.getData("ChildModel", i);
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
            var query = Gql.GqlRequest("query", "GetFeatures");
            var queryFields = Gql.GqlObject("treeItem");
            queryFields.InsertField("Successed");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            provider.featuresModel.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, provider.featuresModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.getData("data");
                if (dataModelLocal.containsKey("GetFeatures")){
                    dataModelLocal = dataModelLocal.getData("GetFeatures");

                    provider.model = dataModelLocal;
                }
            }
        }
    }
}
