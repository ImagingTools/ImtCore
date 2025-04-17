import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
    id: provider;

    property TreeItemModel model: TreeItemModel {};

    // Get all dependencies together with children elements.
    function getAllDependencies(featureId){
        let result = [];

        provider.getAllDependenciesRecursive(featureId, provider.model, result);

        return result;
    }

    // Get dependencies for feature by id.
    function getDependencies(featureId){
        let result = [];

        if (provider.model.containsKey(featureId)){
            let dependencies = provider.model.getData(featureId);
            result =  dependencies.split(';');
        }

        return result;
    }

    //Get all dependent features by feature ID.
    function getAllDependentFeaturesByFeatureId(featureId){
        let result = []

        provider.getAllDependentFeaturesRecursive(featureId, result);

        return result;
    }

    function getAllDependenciesRecursive(featureId, dependenciesModel, retVal){
        console.log("getAllDependenciesRecursive", featureId, dependenciesModel, retVal);
        if (dependenciesModel.containsKey(featureId)){
            let dependencies = dependenciesModel.getData(featureId);

            let dependenciesList = dependencies.split(';');

            for (let i = 0; i < dependenciesList.length; i++){
                let valueId = dependenciesList[i];

                retVal.push(valueId)

                provider.getAllDependenciesRecursive(valueId, dependenciesModel, retVal);
            }
        }
    }

    function getAllDependentFeaturesRecursive(featureId, retVal){
        let keys = provider.model.getKeys();

        for (let i = 0; i < keys.length; i++){
            let key = keys[i];

            let values = provider.model.getData(key);
            if (values != ""){
                let dependenciesList = values.split(';');
                if (dependenciesList.includes(featureId)){
                    retVal.push(key);

                    provider.getAllDependentFeaturesRecursive(key, retVal);
                }
            }
        }
    }
}
