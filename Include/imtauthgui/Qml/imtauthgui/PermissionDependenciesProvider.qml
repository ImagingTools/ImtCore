import QtQuick 2.0
import Acf 1.0

Item {
    id: rolePermissionsContainer;

    property PermissionsProvider permissionsProvider: null;

    function getAllDependencies(featureId){
        if (permissionsProvider == null){
            console.error("ERROR::getAllDependencies - permissionsProvider -> null");

            return;
        }

        let permissionsModel = permissionsProvider.model;
        if (!permissionsModel){
            console.error("ERROR::getAllDependencies - permissionsModel ->", permissionsModel);

            return;
        }

        let dependenciesModel = permissionsModel.GetData("Dependencies");
        if (!dependenciesModel){
            console.error("ERROR::getAllDependencies - dependenciesModel ->", dependenciesModel);

            return;
        }

        let result = []
        privateBlock.getAllDependenciesRecursive(featureId, dependenciesModel, result);

        return result;
    }

    //private functions
    Item {
        id: privateBlock;

        function getAllDependenciesRecursive(featureId, dependenciesModel, retVal){
            if (dependenciesModel.ContainsKey(featureId)){
                let dependencies = dependenciesModel.GetData(featureId);

                let dependenciesList = dependencies.split(';');

                retVal = retVal.concat(dependenciesList)

                for (let i = 0; i < dependenciesList.length; i++){
                    let valueId = dependenciesList[i];

                    getAllDependenciesRecursive(valueId, dependenciesModel, retVal);
                }
            }
        }
    }
}//Container


