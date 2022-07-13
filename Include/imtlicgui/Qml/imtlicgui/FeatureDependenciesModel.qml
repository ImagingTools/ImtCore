import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: featureDependenciesModelContainer;

    property TreeItemModel modelFeatureDependencies;

    onModelFeatureDependenciesChanged: {
        Events.sendEvent("FeatureDependenciesUpdated");
    }

    function reloadModel(){
        dependenciesModel.loadDependModel();
    }

    function updateFeaturesDependenciesAfterPackageEditing(oldPackageId, newPackageId){
//        console.log("updateFeaturesDependenciesAfterPackageEditing", oldPackageId, newPackageId);

//        if (!featureDependenciesModelContainer.modelFeatureDependencies){
//            return;
//        }
//        console.log("1", featureDependenciesModelContainer.modelFeatureDependencies.toJSON());
//        let keys = featureDependenciesModelContainer.modelFeatureDependencies.GetKeys();
//        for (let i = 0; i < keys.length; i++){
//            let value = featureDependenciesModelContainer.modelFeatureDependencies.GetData(keys[i]);

//            let data = keys[i].split('.');

//            let curPackageId = data[0];
//            let curFeatureId = data[1];

//            if (curPackageId == oldPackageId){
//                let newKey = newPackageId + "." + curFeatureId;
//                featureDependenciesModelContainer.modelFeatureDependencies.SetData(newKey, value);
//                featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], "");
//                continue;
//            }

//            let valuesData = value.split(';');

//            for (let j = 0; j < valuesData.length; j++){
//                let valueData = valuesData[j].split('.');

//                let curDependPackageId = valueData[0];
//                let curDependFeatureId = valueData[1];

//                if (curDependPackageId == oldPackageId){
//                    let removeValue = curDependPackageId + '.' + curDependFeatureId;

//                    if (j != valuesData.length - 1){
//                        removeValue += ';';
//                    }
//                    let newValue = value.replace(removeValue, '');
//                    newValue += ';' + newPackageId + "." + curDependFeatureId;
//                    featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], newValue);
//                }
//            }
//        }

//        console.log("2", featureDependenciesModelContainer.modelFeatureDependencies.toJSON());
    }

    function updateFeaturesDependenciesAfterFeatureEditing(packageId, featureOldId, featureNewId, featureNewName){
        console.log("updateFeaturesDependenciesAfterFeatureEditing", packageId, featureOldId, featureNewId, featureNewName);

        if (!featureDependenciesModelContainer.modelFeatureDependencies){
            return;
        }
        console.log("1", featureDependenciesModelContainer.modelFeatureDependencies.toJSON());
        let keys = featureDependenciesModelContainer.modelFeatureDependencies.GetKeys();
        for (let i = 0; i < keys.length; i++){
            let value = featureDependenciesModelContainer.modelFeatureDependencies.GetData(keys[i]);

            let data = keys[i].split('.');

            let curPackageId = data[0];
            let curFeatureId = data[1];

            if (keys[i] == featureOldId){
                featureDependenciesModelContainer.modelFeatureDependencies.SetData(featureNewId, value);
                featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], "");
                break;
            }


//            if (curPackageId == packageId && curFeatureId == featureOldId){
//                let newKey = curPackageId + "." + featureNewId;
//                featureDependenciesModelContainer.modelFeatureDependencies.SetData(newKey, value);
//                featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], "");
//                break;
//            }

            let valuesData = value.split(';');

            for (let j = 0; j < valuesData.length; j++){
//                let valueData = valuesData[j].split('.');

//                let curDependPackageId = valueData[0];
//                let curDependFeatureId = valueData[1];

                if (valuesData[j] == featureOldId){
                    let removeValue = valuesData[j];

                    if (j != valuesData.length - 1){
                        removeValue += ';';
                    }

                    let newValue = value.replace(removeValue, '');
                    newValue += ';' + featureNewId;
                    featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], newValue);
                }

//                if (curDependPackageId == packageId && curDependFeatureId == featureOldId){
//                    let removeValue = curDependPackageId + '.' + curDependFeatureId;

//                    if (j != valuesData.length - 1){
//                        removeValue += ';';
//                    }
//                    let newValue = value.replace(removeValue, '');
//                    newValue += ';' + curPackageId + "." + featureNewId;
//                    featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], newValue);
//                }
            }
        }
        console.log("2", featureDependenciesModelContainer.modelFeatureDependencies.toJSON());
    }

    function removeDependsFeature(packageId, featureId){
        console.log("FeaturesTreeView removeDependsFeature", packageId, featureId);
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

//            if (curPackageId == packageId && curFeatureId == featureId){
//                featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], '');
//                continue;
//            }

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

//                let curDependPackageId = valueData[0];
//                let curDependFeatureId = valueData[1];

//                if (curDependPackageId == packageId && curDependFeatureId == featureId){
//                    let removeValue = curDependPackageId + '.' + curDependFeatureId;

//                    if (j != valuesData.length - 1){
//                        removeValue += ';';
//                    }
//                    let newValue = value.replace(removeValue, '');
//                    featureDependenciesModelContainer.modelFeatureDependencies.SetData(keys[i], newValue);
//                }
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

        function loadDependModel() {
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
