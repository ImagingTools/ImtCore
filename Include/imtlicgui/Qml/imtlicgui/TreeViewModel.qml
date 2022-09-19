import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: treeViewModelContainer;

    property TreeItemModel modelTreeView;

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdate", updateModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdate", updateModel);
    }

    function updateModel(){
        console.log("TreeViewModel updateModel");
        featuresModel.updateModel();
    }

    function updateTreeViewAfterFeatureEditing(packageId, featureOldId, featureNewId, featureNewName){
        console.log("updateTreeViewAfterFeatureEditing", packageId, featureOldId, featureNewId, featureNewName);
        if (!treeViewModelContainer.modelTreeView){
            return;
        }

        console.log("1", treeViewModelContainer.modelTreeView.toJSON());
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let curId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            if (curId == packageId){
                var childModelItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
                if (childModelItems){
                    for (let j = 0; j < childModelItems.GetItemsCount(); j++){
                        let curFeatureId = childModelItems.GetData("Id", j);
                        if (curFeatureId == featureOldId){
                            childModelItems.SetData("Id", featureNewId, j);
                            childModelItems.SetData("Name", featureNewName, j);
                            featureDependenciesModel.featureIdChanged(featureOldId, featureNewId);
                            break;
                        }
                    }
                }

                break;
            }
        }
    }

    function removeFeatureInTreeViewModel(packageId, featureId){
        console.log("FeaturesTreeView removeFeatureInTreeViewModel", packageId, featureId);

        if (!treeViewModelContainer.modelTreeView){
            return;
        }

        for (var i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            var modelChildren = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            var currentPackageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            if (!modelChildren){
                continue;
            }

            for (var j = 0; j < modelChildren.GetItemsCount(); j++){
                var currentFeatureId = modelChildren.GetData("Id", j);

                if (currentPackageId === packageId && currentFeatureId === featureId){
                    console.log("FeaturesTreeView removeFeatureInTreeViewModel Removed", packageId, featureId);
                    modelChildren.RemoveItem(j);

                    featureDependenciesModel.removeDependsFeature(featureDependenciesModel.modelFeatureDependencies, featureId);
                    break;
                }
            }

           treeViewModelContainer.modelTreeView.SetData("ChildModel", modelChildren, i);
        }
    }

    function selectFeature(feature){
        console.log("selectFeature", feature);
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childItems){
                let featureFind = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                   // let value = packageId + "." + featureId;
                    if (feature == featureId){
                        featureFind = true;
                        childItems.SetData("State", 2, j);
                        childItems.SetData("Active", true, j);
                        let downDepends = []
                        featureDependenciesModel.getFeaturesDependsByFeature(feature, downDepends)
                        console.log("downDepends", downDepends);
                        if (downDepends.length > 0){
                            treeViewModelContainer.updateDataFeatureList(downDepends, 2, false);
                        }
                        break;
                    }
                }

                if (featureFind){
                    treeViewModelContainer.modelTreeView.SetData("ChildModel", childItems, i);
                }
            }
        }
    }

    function deselectFeature(feature){
        console.log("deselectFeature", feature);
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childItems){
                let featureFind = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let value = featureId;

                    if (feature == value){
                        featureFind = true;
                        childItems.SetData("State", 0, j);
                        let upDepends = []
                        featureDependenciesModel.getFeaturesDependsByFeature(feature, upDepends)
                        if (upDepends.length > 0){
                            treeViewModelContainer.updateDataFeatureList(upDepends, 0, true);
                        }
                        break;
                    }
                }

                if (featureFind){
                    treeViewModelContainer.modelTreeView.SetData("ChildModel", childItems, i);
                }
            }
        }
    }

    function updateDataFeatureList(features, stateChecked, isActive){
        console.log("updateDataFeatureList");

        if (features.length <= 0){
            return;
        }
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childItems){
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let value = featureId;

                    if (features.indexOf(value) >= 0){
                        console.log("value stateChecked changed ->", stateChecked);
                        childItems.SetData("State", stateChecked, j);
                        childItems.SetData("Active", isActive, j);
                    }
                }
                treeViewModelContainer.modelTreeView.SetData("ChildModel", childItems, i);
            }
        }
    }

    /*
      Сброс всех stateChecked и isActive в дереве
    */
    function resetProperties(){
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let childrenItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childrenItems){
                for (let j = 0; j < childrenItems.GetItemsCount(); j++){
                    childrenItems.SetData("State", 0, j);
                    childrenItems.SetData("Active", true, j);
                    childrenItems.SetData("Visible", true, j);
                }
            }
        }
    }

    function addFeatureInTreeViewModel(packageId, featureId, featureName){
        console.log("FeaturesTreeView addFeatureInTreeViewModel", packageId, featureId, featureName);

        for (var i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            var currentPackageId = treeViewModelContainer.modelTreeView.GetData("Id", i);

            if (currentPackageId === packageId){
                var modelChildren = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);

                if (modelChildren){
                    let childIndex = modelChildren.InsertNewItem();

                    modelChildren.SetData("Id", featureId, childIndex);
                    modelChildren.SetData("Name", featureName, childIndex);
                    modelChildren.SetData("State", 0, childIndex);
                    modelChildren.SetData("Level", 1, childIndex);
                    modelChildren.SetData("Visible", false, childIndex);
                    modelChildren.SetData("Active", true, childIndex);
                    break;
                }
            }
        }
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
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, featuresModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("FeaturesTree")){
                    dataModelLocal = dataModelLocal.GetData("FeaturesTree");
                    if (dataModelLocal.ContainsKey("TreeModel")) {
                        dataModelLocal = dataModelLocal.GetData("TreeModel");

                        treeViewModelContainer.modelTreeView = dataModelLocal;
                        Events.sendEvent("TreeViewModelUpdated");
                    }
                }
            }
        }
    }
}
