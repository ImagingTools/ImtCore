import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: treeViewModelContainer;

    property TreeItemModel modelTreeView;

    onModelTreeViewChanged: {
        Events.sendEvent("FeaturesTreeItemsUpdated");
    }

    function updateModel(){
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
                var childModelItems = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
                if (childModelItems){
                    for (let j = 0; j < childModelItems.GetItemsCount(); j++){
                        let curFeatureId = childModelItems.GetData("Id", j);
                        if (curFeatureId == featureOldId){
                            childModelItems.SetData("Id", featureNewId, j);
                            childModelItems.SetData("Name", featureNewName, j);

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
            var modelChildren = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
            var currentPackageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            if (!modelChildren){
                continue;
            }

            for (var j = 0; j < modelChildren.GetItemsCount(); j++){
                var currentFeatureId = modelChildren.GetData("Id", j);

                if (currentPackageId === packageId && currentFeatureId === featureId){
                    console.log("FeaturesTreeView removeFeatureInTreeViewModel Removed", packageId, featureId);
                    modelChildren.RemoveItem(j);

                    featureDependenciesModel.removeDependsFeature(packageId, featureId);
                    break;
                }
            }

           treeViewModelContainer.modelTreeView.SetData("childItemModel", modelChildren, i);
        }
    }

    function hideCurrentFeatureTreeView(packageId, featureId){
        console.log("FeatureTreeView hideCurrentFeatureTreeView", packageId, featureId);
        if (!treeViewModelContainer.modelTreeView){
            return;
        }
        console.log("1 treeViewModelContainer.modelTreeView", treeViewModelContainer.modelTreeView.toJSON());
        for (var i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let curPackageId = treeViewModelContainer.modelTreeView.GetData("Name", i);
            console.log("curPackageId", curPackageId);
            if (curPackageId == packageId) {
                var childModelItems = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
                if (!childModelItems){
                    continue;
                }

                for (var j = 0; j < childModelItems.GetItemsCount(); j++) {
                    if (childModelItems.GetData("visible", j) === 0){
                        childModelItems.SetData("visible", 1, j);
                    }

                    if (childModelItems.GetData("Id", j) === featureId){
                        console.log("featureId visible false");
                        childModelItems.SetData("visible", 0, j);
                    }
                }
                //treeViewModelContainer.modelTreeView.SetData("childItemModel", childModelItems, i);
                break;
            }
        }

        console.log("2 treeViewModelContainer.modelTreeView", treeViewModelContainer.modelTreeView.toJSON());
    }

    function selectFeature(feature){
        console.log("selectFeature", feature);
       // let dataModelLocal = treeViewModelContainer.modelTreeView;
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
            if (childItems){
                let featureFind = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                   // let value = packageId + "." + featureId;
                    if (feature == featureId){
                        featureFind = true;
                        childItems.SetData("stateChecked", 2, j);
                        childItems.SetData("isActive", 1, j);
                        let downDepends = []
                        featureDependenciesModel.getFeaturesDependsByFeature(feature, downDepends)
                        console.log("downDepends", downDepends);
                        if (downDepends.length > 0){
                            treeViewModelContainer.updateDataFeatureList(downDepends, 2, 0);
                        }
                        break;
                    }
                }

                if (featureFind){
                    treeViewModelContainer.modelTreeView.SetData("childItemModel", childItems, i);
                }
            }
        }
        //treeViewModelContainer.modelTreeView = dataModelLocal;
//        treeViewModelContainer.modelTreeView.Refresh();
    }

    function deselectFeature(feature){
        console.log("deselectFeature", feature);
//        let dataModelLocal = treeViewModelContainer.modelTreeView;
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
            if (childItems){
                let featureFind = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
//                    let value = packageId + "." + featureId;
                    let value = featureId;

                    if (feature == value){
                        featureFind = true;
                        childItems.SetData("stateChecked", 0, j);
                        let upDepends = []
                        featureDependenciesModel.getFeaturesDependsByFeature(feature, upDepends)
                        //console.log("upDepends", upDepends);
                        if (upDepends.length > 0){
                            treeViewModelContainer.updateDataFeatureList(upDepends, 0, 1);
                        }
                        break;
                    }
                }

                if (featureFind){
                    treeViewModelContainer.modelTreeView.SetData("childItemModel", childItems, i);
                }
            }
        }
//        treeViewModelContainer.modelTreeView = dataModelLocal;
//        treeViewModelContainer.modelTreeView.Refresh();
    }

    function updateDataFeatureList(features, stateChecked, isActive){
        console.log("updateDataFeatureList");

        if (features.length <= 0){
            return;
        }
       //let dataModelLocal = treeViewModelContainer.modelTreeView;
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
            if (childItems){
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
//                    let value = packageId + "." + featureId;
                    let value = featureId;

                    if (features.indexOf(value) >= 0){
                        console.log("value stateChecked changed ->", stateChecked);
                        childItems.SetData("stateChecked", stateChecked, j);
                        childItems.SetData("isActive", isActive, j);
                    }
                }
                treeViewModelContainer.modelTreeView.SetData("childItemModel", childItems, i);
            }
        }

//        treeViewModelContainer.modelTreeView.Refresh();
        //treeViewModelContainer.modelTreeView = dataModelLocal;
    }

    /*
      Сброс всех stateChecked и isActive в дереве
    */
    function clearTreeView(){
        //let dataModelLocal = treeViewModelContainer.modelTreeView;
        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let packageId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            let childItems = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
            if (childItems){
                let wasChanged = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let stateChecked = childItems.GetData("stateChecked", j);
                    let isActive = childItems.GetData("isActive", j);
                    let visible = childItems.GetData("visible", j);

                    if (stateChecked == 2){
                        wasChanged = true;
                        childItems.SetData("stateChecked", 0, j);
                    }

                    if (isActive == 0){
                        wasChanged = true;
                        childItems.SetData("isActive", 1, j);
                    }

                    if (visible == 0){
                        wasChanged = true;
                        childItems.SetData("visible", 1, j);
                    }
                }

                if (wasChanged){
                    treeViewModelContainer.modelTreeView.SetData("childItemModel", childItems, i);
                }
            }
        }
        //treeViewModelContainer.modelTreeView = dataModelLocal;
//        treeViewModelContainer.modelTreeView.Refresh();
    }

    function featureIsNew(packageId, featureId){
        if (!treeViewModelContainer.modelTreeView){
            return true;
        }

        if (featureId == ""){
            return true;
        }

        for (let i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            let curId = treeViewModelContainer.modelTreeView.GetData("Id", i);
            if (curId === packageId){
                let modelChild = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);
                if (!modelChild){
                    return true;
                }

                for (let j = 0; j < modelChild.GetItemsCount(); j++){
                    let fId = modelChild.GetData("Id", j);
                    if (fId == featureId){
                        return false;
                    }
                }

                break;
            }
        }

        return true;
    }

    function addFeatureInTreeViewModel(packageId, featureId, featureName){
        console.log("FeaturesTreeView addFeatureInTreeViewModel", packageId, featureId, featureName);

        if (!treeViewModelContainer.modelTreeView){
            console.log("!treeViewModelContainer.modelTreeView return");
            return;
        }

        for (var i = 0; i < treeViewModelContainer.modelTreeView.GetItemsCount(); i++){
            var currentPackageId = treeViewModelContainer.modelTreeView.GetData("Id", i);

            if (currentPackageId === packageId) {
                var modelChildren = treeViewModelContainer.modelTreeView.GetData("childItemModel", i);

                if (!modelChildren){
                    break;
                }
                var childIndex = -1;
                var stateChecked = 0;
                var level = 1;
                var visible = 0;
                var isActive = 1;

                for (var j = 0; j < modelChildren.GetItemsCount(); j++){
                    var currentFeatureId = modelChildren.GetData("Id", j);
                    if (currentFeatureId === featureId){
                        childIndex = j;
                        stateChecked = modelChildren.GetData("stateChecked", j);
                        level = modelChildren.GetData("level", j);
                        visible = modelChildren.GetData("visible", j);
                        isActive = modelChildren.GetData("isActive", j);
                        break;
                    }
                }

                if (childIndex === -1){
                    childIndex = modelChildren.InsertNewItem();
                }

                console.log(childIndex, featureId, featureName, stateChecked, level, visible, isActive, packageId);

                modelChildren.SetData("Id", featureId, childIndex);
                modelChildren.SetData("Name", featureName, childIndex);
                modelChildren.SetData("stateChecked", stateChecked, childIndex);
                modelChildren.SetData("level", level, childIndex);
                modelChildren.SetData("visible", visible, childIndex);
                modelChildren.SetData("isActive", isActive, childIndex);
                modelChildren.SetData("packageId", packageId, childIndex);
                break;
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

//                        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
//                            var modelChildren = dataModelLocal.GetData("childItemModel", i);
//                            dataModelLocal.SetData("childItemModel", modelChildren, i);
//                        }

                        treeViewModelContainer.modelTreeView = dataModelLocal;
                        Events.sendEvent("FeaturesTreeUpdate");
                    }
                }
            }
        }
    }
}
