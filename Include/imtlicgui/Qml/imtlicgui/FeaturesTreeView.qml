import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: featuresTreeViewContainer;

    property TreeItemModel modelTreeItems;
    property TreeItemModel modelDepends;
    property TreeItemModel productLicenseFeatures;
    property var featuresDependencies;

    Component.onCompleted: {
        console.log( "FeaturesTreeView Component.onCompleted");
//        featuresTreeViewContainer.loadFeaturesModel();
//        featuresTreeViewContainer.loadDependModel();
//        featuresTreeViewContainer.loadLicenseDependModel();
    }

    function hideCurrentFeatureTreeView(packageId, featureId){
        console.log("PackageView hideCurrentFeatureTreeView");
        //var modelItems = treeView.modelItems;
        //let dataModelLocal = featuresTreeViewContainer.modelTreeItems;
        if (!featuresTreeViewContainer.modelTreeItems){
            return;
        }

        for (var i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++) {
            if (featuresTreeViewContainer.modelTreeItems.GetData("Name", i) == packageId) {
                var childModelItems = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
                if (!childModelItems){
                    continue;
                }

                for (var j = 0; j < childModelItems.GetItemsCount(); j++) {
                    if (childModelItems.GetData("visible", j) === 0) {
                        childModelItems.SetData("visible", 1, j);
                    }

                    if (childModelItems.GetData("Id", j) === featureId){
                        childModelItems.SetData("visible", 0, j);
                    }
                }
                featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", childModelItems, i);
                break;
            }
        }

        //featuresTreeViewContainer.modelTreeItems = dataModelLocal;
//        featuresTreeViewContainer.modelTreeItems.Refresh();
    }

    function selectFeature(feature){
       // let dataModelLocal = featuresTreeViewContainer.modelTreeItems;
        for (let i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            let packageId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);
            let childItems = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
            if (childItems){
                let featureFind = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let value = packageId + "." + featureId;
                    if (feature == value){
                        featureFind = true;
                        childItems.SetData("stateChecked", 2, j);
                        childItems.SetData("isActive", 1, j);
                        let downDepends = []
                        featuresTreeViewContainer.getFeaturesDependsByFeature(feature, downDepends);
                        console.log("downDepends", downDepends);
                        if (downDepends.length > 0){
                            featuresTreeViewContainer.updateDataFeatureList(downDepends, 2, 0);
                        }
                        break;
                    }
                }

                if (featureFind){
                    featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", childItems, i);
                }
            }
        }
        //featuresTreeViewContainer.modelTreeItems = dataModelLocal;
//        featuresTreeViewContainer.modelTreeItems.Refresh();
    }

    function deselectFeature(feature){
        console.log("deselectFeature", feature);
//        let dataModelLocal = featuresTreeViewContainer.modelTreeItems;
        for (let i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            let packageId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);
            let childItems = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
            if (childItems){
                let featureFind = false;
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let value = packageId + "." + featureId;
                    if (feature == value){
                        featureFind = true;
                        childItems.SetData("stateChecked", 0, j);
                        let upDepends = []
                        featuresTreeViewContainer.getFeaturesDependsByFeature(feature, upDepends);
                        //console.log("upDepends", upDepends);
                        if (upDepends.length > 0){
                            featuresTreeViewContainer.updateDataFeatureList(upDepends, 0, 1);
                        }
                        break;
                    }
                }

                if (featureFind){
                    featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", childItems, i);
                }
            }
        }
//        featuresTreeViewContainer.modelTreeItems = dataModelLocal;
//        featuresTreeViewContainer.modelTreeItems.Refresh();
    }

    /*
      Вернуть список feature от которых зависит переданная
    */
    function getFeaturesDependsByFeature(key, list){
        console.log("getFeaturesDependsByFeature", key, list);
        if (featuresTreeViewContainer.modelDepends.ContainsKey(key)){
            let valuesStr = featuresTreeViewContainer.modelDepends.GetData(key);
            if (valuesStr){
                let values = valuesStr.split(';');
                for (let value of values){
                    list.push(value);
                }

                for (let value of values) {
                    featuresTreeViewContainer.getFeaturesDependsByFeature(value, list);
                }
            }
        }
    }

    /*
      Вернуть список feature зависящих от переданной
    */
    function getFeaturesDependsByFeatureUp(key, list){
        console.log("getFeaturesDependsByFeatureUp", key, list);
        var arr = featuresTreeViewContainer.modelDepends.GetKeys();
        for (let curKey of featuresTreeViewContainer.modelDepends.GetKeys()) {
            let valuesStr = featuresTreeViewContainer.modelDepends.GetData(curKey);
            if (valuesStr){
                let values = valuesStr.split(";");
                if (values.indexOf(key) > -1){
                    list.push(curKey);
                    featuresTreeViewContainer.getFeaturesDependsByFeatureUp(curKey, list);
                }
            }
        }
    }

    function updateDataFeatureList(features, stateChecked, isActive){
        console.log("updateDataFeatureList");

        if (features.length <= 0){
            return;
        }
       //let dataModelLocal = featuresTreeViewContainer.modelTreeItems;
        for (let i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            let packageId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);
            let childItems = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
            if (childItems){
                for (let j = 0; j < childItems.GetItemsCount(); j++){
                    let featureId = childItems.GetData("Id", j);
                    let value = packageId + "." + featureId;

                    if (features.indexOf(value) >= 0){
                        childItems.SetData("stateChecked", stateChecked, j);
                        childItems.SetData("isActive", isActive, j);
                    }
                }
                featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", childItems, i);
            }
        }

//        featuresTreeViewContainer.modelTreeItems.Refresh();
        //featuresTreeViewContainer.modelTreeItems = dataModelLocal;
    }

    /*
      Сброс всех stateChecked и isActive в дереве
    */
    function clearTreeView(){
        //let dataModelLocal = featuresTreeViewContainer.modelTreeItems;
        for (let i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            let packageId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);
            let childItems = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
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
                    featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", childItems, i);
                }
            }
        }
        //featuresTreeViewContainer.modelTreeItems = dataModelLocal;
//        featuresTreeViewContainer.modelTreeItems.Refresh();
    }

    function loadLicenseDependModel(){
        licensesDependenciesModel.loadLicenseDependModel();
    }

    function printInfo() {
        for (var i = 0; i < featuresTreeViewContainer.modelDepends.GetItemsCount(); i++) {
            console.log("Package ID", featuresTreeViewContainer.modelDepends.GetData("RootPackageId", i));
            console.log("Feature ID", featuresTreeViewContainer.modelDepends.GetData("RootFeatureId", i));
            var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", i);

            for (var j = 0; j < modelPackages.GetItemsCount(); j++) {
                console.log("\tPackage ID", modelPackages.GetData("Id", j));
                var modelChildren = modelPackages.GetData("childItemModel", j);

                for (var k = 0; k < modelChildren.GetItemsCount(); k++) {
                    console.log("\t\tFeature ID", modelChildren.GetData("Id", k));
                }
            }
        }

        console.log("modelTreeItems", modelTreeItems.toJSON());
    }

    function featureIsNew(packageId, featureId){
        if (!featuresTreeViewContainer.modelTreeItems){
            return false;
        }

        if (featureId == ""){
            return true;
        }

        for (let i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            let curId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);
            if (curId === packageId){
                let modelChild = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
                if (!modelChild){
                    return false;
                }

                for (let j = 0; j < modelChild.GetItemsCount(); j++){
                    let fId = modelChild.GetData("Id", j);
                    if (fId == featureId){
                        return true;
                    }
                }

                break;
            }
        }

        return false;
    }

    function addFeatureInTreeViewModel(packageId, featureId, featureName){
        console.log("FeaturesTreeView addFeatureInTreeViewModel", packageId, featureId, featureName);

        if (!featuresTreeViewContainer.modelTreeItems){
            console.log("!featuresTreeViewContainer.modelTreeItems return");
            return;
        }

        for (var i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            var currentPackageId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);

            if (currentPackageId === packageId) {
                var modelChildren = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);

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

                featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", modelChildren, i);
                break;
            }
        }
    }

    function removeFeatureInTreeViewModel(packageId, featureId){
        console.log("FeaturesTreeView removeFeatureInTreeViewModel", packageId, featureId);

        if (!featuresTreeViewContainer.modelTreeItems){
            return;
        }

        for (var i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            var modelChildren = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", i);
            var currentPackageId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);
            if (!modelChildren){
                continue;
            }

            for (var j = 0; j < modelChildren.GetItemsCount(); j++){
                var currentFeatureId = modelChildren.GetData("Id", j);

                if (currentPackageId === packageId && currentFeatureId === featureId){
                    console.log("FeaturesTreeView removeFeatureInTreeViewModel Removed", packageId, featureId);
                    modelChildren.RemoveItem(j);

                    featuresTreeViewContainer.removeDependsFeature(packageId, featureId);
                    break;
                }
            }

           featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", modelChildren, i);
        }
    }

    function removeDependsFeature(packageId, featureId){
        console.log("FeaturesTreeView removeDependsFeature", packageId, featureId);
        if (!featuresTreeViewContainer.modelDepends ||!featuresTreeViewContainer.modelDepends.GetItemsCount() === 0){
            return;
        }

        for (var i = 0; i < featuresTreeViewContainer.modelDepends.GetItemsCount(); i++){
            var rootFeatureId = featuresTreeViewContainer.modelDepends.GetData("RootFeatureId", i);
            var rootPackageId = featuresTreeViewContainer.modelDepends.GetData("RootPackageId", i);

            if (packageId === rootPackageId && rootFeatureId === featureId){
                console.log("FeaturesTreeView removeDependsFeature Removed", packageId, featureId);
                featuresTreeViewContainer.modelDepends.RemoveItem(i);
                break;
            }

            var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", i);

            for (var j = 0; j < modelPackages.GetItemsCount(); j++){
                var currentPackageId = modelPackages.GetData("Id", j);

                var currentChildModel = modelPackages.GetData("childItemModel", j);

                if (!currentChildModel || currentChildModel.GetItemsCount() === 0){
                    continue;
                }

                for (var k = 0; k < currentChildModel.GetItemsCount(); k++){
                    var currentFeatureId = currentChildModel.GetData("Id", k);

                    if (currentPackageId === packageId && currentFeatureId === featureId){
                        console.log("FeaturesTreeView removeDependsFeature Removed", packageId, featureId);
                        currentChildModel.RemoveItem(k);
                    }
                }

                modelPackages.SetData("childItemModel", currentChildModel, j)

                if (currentChildModel.GetItemsCount() === 0){
                    modelPackages.RemoveItem(j);
                }
            }

            featuresTreeViewContainer.modelDepends.SetData("Packages", modelPackages, i);

            if (modelPackages.GetItemsCount() === 0){
                featuresTreeViewContainer.modelDepends.RemoveItem(i);
            }
        }

    }

    function loadFeaturesModel() {
        console.log( "FeaturesTreeView loadFeaturesModel()");
        featuresModel.loadFeaturesModel();
    }

    function loadDependModel() {
        console.log( "FeaturesTreeView loadDependModel()");
        dependenciesModel.loadDependModel();
    }

    GqlModel {
        id: featuresModel;

        function loadFeaturesModel() {
            console.log( "FeaturesTreeView GqlModel loadFeaturesModel");
            var query = Gql.GqlRequest("query", "FeaturesTree");
            var queryFields = Gql.GqlObject("treeItem");
            queryFields.InsertField("TreeModel");
            query.AddField(queryFields);
            var gqlData = query.GetQuery();
            console.log("TreeView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, featuresModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("FeaturesTree")) {
                    dataModelLocal = dataModelLocal.GetData("FeaturesTree");
                    if (dataModelLocal.ContainsKey("TreeModel")) {
                        dataModelLocal = dataModelLocal.GetData("TreeModel");

//                        var index = featuresTreeViewContainer.getIndexFromTreeViewByPackageId(featuresTreeViewContainer.packageItem.itemId);
//                        var modelChildren;

//                        if (index !== -1){
//                            modelChildren = featuresTreeViewContainer.modelTreeItems.GetData("childItemModel", index);
//                        }

                        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
                            var modelChildren = dataModelLocal.GetData("childItemModel", i);
                            dataModelLocal.SetData("childItemModel", modelChildren, i);
                        }

                        console.log( "FeaturesTreeView GqlModel assign a new modelTreeItems");
                        featuresTreeViewContainer.modelTreeItems = dataModelLocal;
                        console.log();

//                        if (index !== -1){
//                            featuresTreeViewContainer.modelTreeItems.SetData("childItemModel", modelChildren, index);
//                        }
                    }
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

                        featuresTreeViewContainer.modelDepends = dataModelLocal;
                    }
                }
            }
        }
    }

    GqlModel {
        id: licensesDependenciesModel;

        function loadLicenseDependModel() {
            console.log( "FeaturesTreeView GqlModel loadLicenseDependModel");
            var query = Gql.GqlRequest("query", "LicensesDependencies");
            var queryFields = Gql.GqlObject("dependencies");
            queryFields.InsertField("TreeModel");
            query.AddField(queryFields);
            var gqlData = query.GetQuery();
            console.log("TreeView loadLicenseDependModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, dependenciesModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");

                if (dataModelLocal.ContainsKey("LicensesDependencies")){
                    dataModelLocal = dataModelLocal.GetData("LicensesDependencies");

                    if (dataModelLocal.ContainsKey("TreeModel")){
                        dataModelLocal = dataModelLocal.GetData("TreeModel");
                    }

                    featuresTreeViewContainer.productLicenseFeatures = dataModelLocal;
                }
            }
        }
    }
}
