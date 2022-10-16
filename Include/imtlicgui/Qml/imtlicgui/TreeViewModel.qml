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

                    featureDependenciesModel.removeDependsFeature(featureDependenciesModel.model, featureId);
                    break;
                }
            }

           treeViewModelContainer.modelTreeView.SetData("ChildModel", modelChildren, i);
        }
    }

    function selectFeature(feature){
        console.log("selectFeature", feature);

        recursiveSelection(modelTreeView, feature);
//        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
//            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
//            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
//            if (childItems){
//                for (let j = 0; j < childItems.GetItemsCount(); j++){
//                    let featureId = childItems.GetData("Id", j);
//                    if (feature == featureId){
//                        childItems.SetData("State", Qt.Checked, j);
//                        childItems.SetData("Active", true, j);
//                    }
//                }
//            }
//        }
    }

    function recursiveSelection(model, featureId){
         for (let i = 0; i < model.GetItemsCount(); i++){
             let itemId = model.GetData("Id", i);

             if (itemId == featureId){
                 model.SetData("State", Qt.Checked, i);
                 model.SetData("Active", true, i);

                 return;
             }

             let childModel = model.GetData("ChildModel", i);

             if (childModel){
                 recursiveSelection(childModel, featureId);
             }
         }
    }

//    function selectFeature(feature){
//        console.log("selectFeature", feature);
//        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
//            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
//            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
//            if (childItems){
//                for (let j = 0; j < childItems.GetItemsCount(); j++){
//                    let featureId = childItems.GetData("Id", j);
//                    if (feature == featureId){
//                        childItems.SetData("State", 2, j);
//                        childItems.SetData("Active", true, j);
//                        let downDepends = []
//                        featureDependenciesModel.getAllChildrenDependsFeatures(feature, downDepends)
//                        if (downDepends.length > 0){
//                            treeViewModelContainer.updateDataFeatureList(downDepends, 2, false);
//                        }
//                        break;
//                    }
//                }
//            }
//        }
//    }

    function deselectFeature(feature){
        console.log("deselectFeature", feature);
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childItems){
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let value = featureId;

                    if (feature == value){

                        childItems.SetData("State", 0, j);
                        let upDepends = []
                        featureDependenciesModel.getAllChildrenDependsFeatures(feature, upDepends)

                        if (upDepends.length > 0){
                            treeViewModelContainer.updateDataFeatureList(upDepends, 0, true);
                        }

                        break;
                    }
                }
            }
        }
    }

    function setValueToProperty(prop, value, features){
        if (features.length == 0){
            return;
        }

        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childItems){
                _setValueToPropertyRecursive(childItems, prop, value, features);
            }
        }
    }

    function _setValueToPropertyRecursive(model, prop, value, features){
        console.log("_makeFeaturesInactiveRecursive", model, features);

        for (let i = 0; i < model.GetItemsCount(); i++){
            let featureId = model.GetData("Id", i);

            if (features.includes(featureId)){

                if (model.ContainsKey(prop)){
                    console.log("model SetDAta", prop, value);
                    model.SetData(prop, value, i);
                }
            }

            let childModel = model.GetData("ChildModel", i);

            if (childModel){
                _setValueToPropertyRecursive(childModel, prop, value, features);
            }
        }
    }


    function resetProperties(){
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let childrenItems = treeViewModelContainer.modelTreeView.GetData("ChildModel", i);
            if (childrenItems){
                _resetRecursive(childrenItems);
            }
        }
    }

    function _resetRecursive(model){
         for (let i = 0; i < model.GetItemsCount(); i++){             
             let state = model.GetData("State", i);
             let isActive = model.GetData("Active", i);
             let isVisible = model.GetData("Visible", i);

             if (state != Qt.Unchecked){
                 model.SetData("State", Qt.Unchecked, i);
             }

             if (!isActive){
                 model.SetData("Active", true, i);
             }

             if (!isVisible){
                 model.SetData("Visible", true, i);
             }

             let childModel = model.GetData("ChildModel", i);
             if (childModel){
                 _resetRecursive(childModel);
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

            Events.sendEvent("TreeViewModelUpdateStarted");
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
                        Events.sendEvent("TreeViewModelUpdateFinished");
                    }
                }
            }
        }
    }
}
