import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: featuresTreeViewContainer;

    property TreeItemModel modelTreeItems;
    property TreeItemModel modelDepends;
    property TreeItemModel productLicenseFeatures;

    Component.onCompleted: {
        console.log( "FeaturesTreeView Component.onCompleted");
        featuresTreeViewContainer.loadFeaturesModel();
        featuresTreeViewContainer.loadDependModel();

        licensesDependenciesModel.loadLicenseDependModel();
    }

    function printInfo() {
        for (var i = 0; i < featuresTreeViewContainer.modelDepends.GetItemsCount(); i++) {
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
    }

    function getIndexFromTreeViewByPackageId(packageId){

        if (!featuresTreeViewContainer.modelTreeItems){
            return -1;
        }

        for (var i = 0; i < featuresTreeViewContainer.modelTreeItems.GetItemsCount(); i++){
            var curId = featuresTreeViewContainer.modelTreeItems.GetData("Id", i);

            if (curId === packageId){
                return i;
            }
        }

        return -1;
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
                    var currentStateChecked = modelChildren.GetData("stateChecked", j);
                    var currentLevel = modelChildren.GetData("level", j);
                    var currentVisible = modelChildren.GetData("visible", j);
                    var currentIsActive = modelChildren.GetData("isActive", j);

                    if (currentFeatureId === featureId){
                        childIndex = j;
                        stateChecked = currentStateChecked;
                        level = currentLevel;
                        visible = currentVisible;
                        isActive = currentIsActive;
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

    function getPackageIndexByPackageId(packageId, rootIndex) {
        var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);

        if (!modelPackages) {
            return -1;
        }

        for (var i = 0; i < modelPackages.GetItemsCount(); i++) {
            if (modelPackages.GetData("Id", i) === packageId) {
                return i;
            }
        }

        return -1;
    }

    function addNewPackageToRootFeatureByRootIndex(packageId, rootIndex) {
        var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);
        var index = modelPackages.InsertNewItem();
        modelPackages.SetData("Id", packageId, index);
        featuresTreeViewContainer.modelDepends.SetData("Packages", modelPackages, rootIndex)
        return index;
    }

    function getIndexByRootFeatureId(featureId) {
        console.log("FeaturesTreeView getIndexByRootFeatureId", featureId)

        for (var i = 0; i < featuresTreeViewContainer.modelDepends.GetItemsCount(); i++) {
            if (featuresTreeViewContainer.modelDepends.GetData("RootFeatureId", i) === featureId) {
                return i;
            }
        }

        return -1;
    }

    function getIndexByPackageId(rootIndex, packageId) {
        console.log("FeaturesTreeView getIndexByPackageId", rootIndex, packageId);

        var packages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);

        if (!packages) {
            console.log("FeaturesTreeView getIndexByPackageId return -1");
            return -1;
        }

        for (var i = 0; i < packages.GetItemsCount(); i++) {
            if (packages.GetData("Id", i) === packageId) {
                console.log("FeaturesTreeView getIndexByPackageId return", i);
                return i;
            }
        }

        console.log("FeaturesTreeView getIndexByPackageId return -1");
        return -1;
    }

    function featureIdHasDependency(featureId) {
        if (!featuresTreeViewContainer.modelDepends) {
            return false;
        }

        for (var i = 0; i < featuresTreeViewContainer.modelDepends.GetItemsCount(); i++) {
            if (featuresTreeViewContainer.modelDepends.GetData("RootFeatureId", i) === featureId) {
                return true;
            }
        }

        return false;
    }

    function rootFeatureIdHasDependPackage(rootIndex) {
        var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);

        if (!modelPackages) {
            return false;
        }
        return true;
    }

    function dependModelPackageHasChild(rootIndex, packageIndex) {
        var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);
        var modelChildren = modelPackages.GetData("childItemModel", packageIndex);

        if (modelChildren && modelChildren.GetItemsCount() > 0) {
            return true;
        }
        return false;
    }

    function dependModelRemovePackageByIndex(rootIndex, packageIndex) {
        var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);
        modelPackages.RemoveItem(packageIndex);
        featuresTreeViewContainer.modelDepends.SetData("Packages", modelPackages,  rootIndex);
    }

    function dependModelRemoveRootFeatureByIndex(rootIndex) {
        featuresTreeViewContainer.modelDepends.RemoveItem(rootIndex);
    }

    function dependModePackageGetIndexByFeatureId(rootIndex, packageIndex, featureId) {
        var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", rootIndex);

        var modelChildren = modelPackages.GetData("childItemModel", packageIndex);

        if (!modelPackages || !modelChildren) {
            return -1;
        }

        for (var i = 0; i < modelChildren.GetItemsCount(); i++) {
            if (modelChildren.GetData("Id", i) === featureId) {
                return i;
            }
        }

        return -1;
    }

    function findInAllRootFeaturesDependFeatureById(packageId, featureId) {

        console.log("FeaturesTreeView findInAllRootFeaturesDependFeatureById", packageId, featureId);
        if (!featuresTreeViewContainer.modelDepends) {
            return false;
        }

        var result = [];
        for (var i = 0; i < featuresTreeViewContainer.modelDepends.GetItemsCount(); i++) {
            var modelPackages = featuresTreeViewContainer.modelDepends.GetData("Packages", i);
            var rootFeatureId = featuresTreeViewContainer.modelDepends.GetData("RootFeatureId", i);
            var rootPackageId = featuresTreeViewContainer.modelDepends.GetData("RootPackageId", i);
            if (!modelPackages) {
                continue;
            }

            for (var j = 0; j < modelPackages.GetItemsCount(); j++) {
                var pId = modelPackages.GetData("Id", j);
                if (pId === packageId) {
                    var modelChildren = modelPackages.GetData("childItemModel", j);

                    if (!modelChildren) {
                        continue;
                    }

                    for (var k = 0; k < modelChildren.GetItemsCount(); k++) {
                        var fId = modelChildren.GetData("Id", k);

                        if (fId === featureId) {
                            result.push(rootPackageId + "." + rootFeatureId);

                            break;
                            //return true;
                        }
                    }
                    break;
                }
            }
        }

        return result;
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

                if (dataModelLocal.ContainsKey("LicensesDependencies")) {
                    dataModelLocal = dataModelLocal.GetData("LicensesDependencies");

                    if (dataModelLocal.ContainsKey("TreeModel")) {
                        dataModelLocal = dataModelLocal.GetData("TreeModel");
                        featuresTreeViewContainer.productLicenseFeatures = dataModelLocal;
                    }
                }
            }
        }
    }
}
