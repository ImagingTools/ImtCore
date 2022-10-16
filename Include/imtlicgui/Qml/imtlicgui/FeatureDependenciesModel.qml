import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: featureDependenciesModelContainer;

    property TreeItemModel model;

    Component.onCompleted: {
        Events.subscribeEvent("FeatureDependenciesUpdate", updateModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("FeatureDependenciesUpdate", updateModel);
    }

    function updateModel(){
        dependenciesModel.updateModel();
    }

    function featureIdChanged(oldId, newId){
        console.log("featureIdChanged", oldId, newId);
        console.log("featureDependenciesModelContainer.model 1", featureDependenciesModelContainer.model.toJSON());

        let keys = featureDependenciesModelContainer.model.GetKeys();
        for (let i = 0; i < keys.length; i++){
            let key = keys[i];
            let value = featureDependenciesModelContainer.model.GetData(key);

            if (key == oldId){
                console.log("Key changed",key, oldId);
                featureDependenciesModelContainer.model.SetData(newId, value);
//                featureDependenciesModelContainer.model.SetData(key, "");
                featureDependenciesModelContainer.model.RemoveData(key);
                break;
            }

            let valuesData = value.split(';');

            for (let j = 0; j < valuesData.length; j++){
                if (valuesData[j] == oldId){
                    let removeValue = valuesData[j];

                    if (j != valuesData.length - 1){
                        removeValue += ';';
                    }

                    let newValue = value.replace(removeValue, '');
                    newValue += ';' + newId;
                    featureDependenciesModelContainer.model.SetData(key, newValue);
                }
            }
        }

        featureDependenciesModelContainer.model.Refresh();
        console.log("featureDependenciesModelContainer.model 2", featureDependenciesModelContainer.model.toJSON());

    }

    function removeDependsFeature(model, featureId){
        console.log("FeaturesTreeView removeDependsFeature", featureId);


        if (!model){
            return;
        }

        let keys = model.GetKeys();

        for (let i = 0; i < keys.length; i++){
            let value = model.GetData(keys[i]);

            if (keys[i] == featureId){
                model.RemoveData(keys[i]);
                continue;
            }

            let valuesData = value.split(';');

            for (let j = 0; j < valuesData.length; j++){

                if (valuesData[j] == featureId){
                    let removeValue = valuesData[j];

                    if (j != 0){
                        removeValue = ';' + removeValue;
                    }
                    else{
                        removeValue += ';';
                    }

                    let newValue = value.replace(removeValue, '');

                    if (newValue == ""){
                        model.RemoveData(keys[i]);
                    }
                    else{
                        model.SetData(keys[i], newValue);
                    }
                }
            }
        }
    }

    function getDependsFeatures(key){
        let result = []

        if (model.ContainsKey(key)){
            let valuesStr = model.GetData(key);

            let values = valuesStr.split(';');
            for (let value of values){
                result.push(value);
            }
        }

        return result;
    }

    function getParentFeatures(key){
        console.log("getParentFeature", key);
        console.log("model", model.toJSON());

        let result = []
        for (let curKey of model.GetKeys()) {
            let valuesStr = model.GetData(curKey);
            if (valuesStr){
                let values = valuesStr.split(";");

                let index = values.indexOf(key);

                if (index > -1){
                    result.push(curKey);
                }
            }
        }

        return result;
    }

    /*
      Вернуть список feature от которых зависит переданная
    */
    function getAllChildrenDependsFeatures(key, list){
        console.log("getAllChildrenDependsFeatures", key, list);

        if (!model){
            return;
        }

        if (model.ContainsKey(key)){
            let dependenciesModel = model.GetData(key);

            for (let i = 0; i < dependenciesModel.GetItemsCount(); i++){
                let featureId = dependenciesModel.GetData("Id", i);

                list.push(featureId);

                getAllChildrenDependsFeatures(featureId, list);
            }
        }
    }

    /*
      Вернуть список feature зависящих от переданной
    */
    function getAllParentsDependsFeatures(key, list){
        console.log("getAllParentsDependsFeatures", key, list);

        if (!model){
            return;
        }

        var keys = model.GetKeys();
        for (let curKey of keys){
            let dependenciesModel = model.GetData(curKey);
            if (dependenciesModel){
                for (let i = 0; i < dependenciesModel.GetItemsCount(); i++){
                    let featureId = dependenciesModel.GetData("Id", i);

                    if (featureId == key){
                        list.push(curKey);
                        getAllParentsDependsFeatures(curKey, list);
                    }
                }
            }
        }
    }

    GqlModel {
        id: dependenciesModel;

        function updateModel() {
            console.log( "FeaturesTreeView GqlModel loadDependModel");
            var query = Gql.GqlRequest("query", "FeaturesDependencies");
            var queryFields = Gql.GqlObject("dependencies");
            queryFields.InsertField("TreeModel");
            query.AddField(queryFields);
            var gqlData = query.GetQuery();
            console.log("TreeView depend query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, dependenciesModel);
            if (this.state === "Ready"){
                console.log("FeaturesDependencies READY", dependenciesModel.toJSON());
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("FeaturesDependencies")) {
                    dataModelLocal = dataModelLocal.GetData("FeaturesDependencies");

                    model = dataModelLocal;
                    Events.sendEvent("FeatureDependenciesUpdated");
                }
                else{
                    dataModelLocal = dependenciesModel.AddTreeModel("FeaturesDependencies");

                    model = dataModelLocal;
                    Events.sendEvent("FeatureDependenciesUpdated");

                }
            }
        }
    }
}
