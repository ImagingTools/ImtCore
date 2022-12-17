import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: provider;

    property TreeItemModel model: TreeItemModel {};

    // Update feature dependencies model.
    function updateModel(){
        dependenciesModel.updateModel();
    }

    // Get all dependencies together with children elements.
    function getAllDependencies(featureId){
        console.log("getAllDependencies", featureId);

        let result = []

        privateBlock.getAllDependenciesRecursive(featureId, provider.model, result);

        console.log("result", result);

        return result;
    }

    // Get dependencies for feature by id.
    function getDependencies(featureId){
        let result = []

        if (model.ContainsKey(featureId)){
            let dependencies = dependenciesModel.GetData(featureId);
            result =  dependencies.split(';');
        }

        return result;
    }

    //Get all dependent features by feature ID.
    function getAllDependentFeaturesByFeatureId(featureId){
        let result = []

        privateBlock.getAllDependentFeaturesRecursive(featureId, result);

        return result;
    }

    // private functions
    Item {
        id: privateBlock;

        function getAllDependenciesRecursive(featureId, dependenciesModel, retVal){
            console.log("getAllDependenciesRecursive", featureId, dependenciesModel, retVal);
            if (dependenciesModel.ContainsKey(featureId)){
                let dependencies = dependenciesModel.GetData(featureId);

                let dependenciesList = dependencies.split(';');

                for (let i = 0; i < dependenciesList.length; i++){
                    let valueId = dependenciesList[i];

                    retVal.push(valueId)

                    getAllDependenciesRecursive(valueId, dependenciesModel, retVal);
                }
            }
        }

        function getAllDependentFeaturesRecursive(featureId, retVal){
            let keys = provider.model.GetKeys();

            for (let i = 0; i < keys.length; i++){
                let key = keys[i];

                let values = provider.model.GetData(key);
                if (values != ""){
                    let dependenciesList = values.split(';')
                    if (dependenciesList.includes(featureId)){
                        retVal.push(key)

                        getAllDependentFeaturesRecursive(key, retVal)
                    }
                }
            }
        }
    }

    // GqlModel for update feature dependencies model.
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
