import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: featureDependenciesModelContainer;

    property TreeItemModel modelFeatureDependencies;

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
        console.log("featureDependenciesModelContainer.modelFeatureDependencies 1", featureDependenciesModelContainer.modelFeatureDependencies.toJSON());

        let keys = featureDependenciesModelContainer.modelFeatureDependencies.GetKeys();
        for (let i = 0; i < keys.length; i++){
            let key = keys[i];
            let value = featureDependenciesModelContainer.modelFeatureDependencies.GetData(key);

            if (key == oldId){
                console.log("Key changed",key, oldId);
                featureDependenciesModelContainer.modelFeatureDependencies.SetData(newId, value);
//                featureDependenciesModelContainer.modelFeatureDependencies.SetData(key, "");
                featureDependenciesModelContainer.modelFeatureDependencies.RemoveData(key);
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
                    featureDependenciesModelContainer.modelFeatureDependencies.SetData(key, newValue);
                }
            }
        }

        featureDependenciesModelContainer.modelFeatureDependencies.Refresh();
        console.log("featureDependenciesModelContainer.modelFeatureDependencies 2", featureDependenciesModelContainer.modelFeatureDependencies.toJSON());

    }

    function removeDependsFeature(featureId){
        console.log("FeaturesTreeView removeDependsFeature", featureId);
        if (!featureDependenciesModelContainer.modelFeatureDependencies){
            return;
        }

        let keys = featureDependenciesModelContainer.modelFeatureDependencies.GetKeys();

        for (let i = 0; i < keys.length; i++){
            let value = featureDependenciesModelContainer.modelFeatureDependencies.GetData(keys[i]);
            let data = keys[i].split('.');

            let curPackageId = data[0];
            let curFeatureId = data[1];

            if (keys[i] == featureId){
                featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], '');
                continue;
            }
            let valuesData = value.split(';');

            for (let j = 0; j < valuesData.length; j++){
                let valueData = valuesData[j].split('.');

                if (valuesData[j] == featureId){
                    let removeValue = valuesData[j];

                    if (j != valuesData.length - 1){
                        removeValue += ';';
                    }
                    let newValue = value.replace(removeValue, '');
                    featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], newValue);
                }
            }
        }
    }

    /*
      Вернуть список feature от которых зависит переданная
    */
    function getFeaturesDependsByFeature(key, list){
        console.log("getFeaturesDependsByFeature", key, list);
        if (featureDependenciesModelContainer.modelFeatureDependencies.ContainsKey(key)){
            let valuesStr = featureDependenciesModelContainer.modelFeatureDependencies.GetData(key);
            if (valuesStr){
                let values = valuesStr.split(';');
                for (let value of values){
                    list.push(value);
                }

                for (let value of values) {
                    featureDependenciesModelContainer.getFeaturesDependsByFeature(value, list);
                }
            }
        }
    }

    /*
      Вернуть список feature зависящих от переданной
    */
    function getFeaturesDependsByFeatureUp(key, list){
        console.log("getFeaturesDependsByFeatureUp", key, list);
        var arr = featureDependenciesModelContainer.modelFeatureDependencies.GetKeys();
        for (let curKey of featureDependenciesModelContainer.modelFeatureDependencies.GetKeys()) {
            let valuesStr = featureDependenciesModelContainer.modelFeatureDependencies.GetData(curKey);
            if (valuesStr){
                let values = valuesStr.split(";");
                if (values.indexOf(key) > -1){
                    list.push(curKey);
                    featureDependenciesModelContainer.getFeaturesDependsByFeatureUp(curKey, list);
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
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("FeaturesDependencies")) {
                    dataModelLocal = dataModelLocal.GetData("FeaturesDependencies");
                    if (dataModelLocal.ContainsKey("TreeModel")) {
                        dataModelLocal = dataModelLocal.GetData("TreeModel");

                        featureDependenciesModelContainer.modelFeatureDependencies = dataModelLocal;
                    }
                }
            }
        }
    }
}
