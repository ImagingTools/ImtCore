import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: container;
    property TreeItemModel model;

    property TreeItemModel dependModel;
    //property alias dependModel: dependModel;

//    TreeItemModel {
//        id: dependModel;
//    }

    Component.onCompleted: {
        console.log( "FeaturesTreeView Component.onCompleted");
        container.loadFeaturesModel();
        container.loadDependModel();
    }

    onDependModelChanged: {
        console.log( "FeaturesTreeView onDependModelChanged");
    }

    function printInfo() {
        for (var i = 0; i < container.dependModel.GetItemsCount(); i++) {
            console.log("Feature ID", container.dependModel.GetData("RootFeatureId", i));
            var packageModel = container.dependModel.GetData("Packages", i);

            for (var j = 0; j < packageModel.GetItemsCount(); j++) {
                console.log("\tPackage ID", packageModel.GetData("Id", j));
                var childModel = packageModel.GetData("childItemModel", j);

                for (var k = 0; k < childModel.GetItemsCount(); k++) {
                    console.log("\t\tFeature ID", childModel.GetData("Id", k));
                }
            }
        }
    }

    function addFeatureInTreeViewModel(packageId, featureId, featureName){
        console.log("FeaturesTreeView addFeatureInTreeViewModel", packageId, featureId, featureName);

        if (!container.model){
            return;
        }

        for (var i = 0; i < container.model.GetItemsCount(); i++){
            var currentPackageId = container.model.GetData("Id", i);

            if (currentPackageId === packageId) {
                var childModel = container.model.GetData("childItemModel", i);

                if (!childModel){
                    break;
                }
                var childIndex = -1;
                var stateChecked = 0;
                var level = 1;
                var visible = 0;
                var isActive = 1;

                for (var j = 0; j < childModel.GetItemsCount(); j++){
                    var currentFeatureId = childModel.GetData("Id", j);
                    var currentStateChecked = childModel.GetData("stateChecked", j);
                    var currentLevel = childModel.GetData("level", j);
                    var currentVisible = childModel.GetData("visible", j);
                    var currentIsActive = childModel.GetData("isActive", j);

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
                    childIndex = childModel.InsertNewItem();
                }

                childModel.SetData("Id", featureId, childIndex);
                childModel.SetData("Name", featureName, childIndex);
                childModel.SetData("stateChecked", stateChecked, childIndex);
                childModel.SetData("level", level, childIndex);
                childModel.SetData("visible", visible, childIndex);
                childModel.SetData("isActive", isActive, childIndex);
                childModel.SetData("packageId", packageId, childIndex);

                container.model.SetData("childItemModel", childModel, i);
                break;
            }
        }
    }

    function removeFeatureInTreeViewModel(packageId, featureId){
        console.log("FeaturesTreeView removeFeatureInTreeViewModel", packageId, featureId);

        if (!container.model){
            return;
        }

        for (var i = 0; i < container.model.GetItemsCount(); i++){
            var childModel = container.model.GetData("childItemModel", i);
            var currentPackageId = container.model.GetData("Id", i);
            if (!childModel){
                continue;
            }

            for (var j = 0; j < childModel.GetItemsCount(); j++){
                var currentFeatureId = childModel.GetData("Id", j);

                if (currentPackageId === packageId && currentFeatureId === featureId){
                    console.log("FeaturesTreeView removeFeatureInTreeViewModel Removed", packageId, featureId);
                    childModel.RemoveItem(j);

                    container.removeDependsFeature(packageId, featureId);
                    break;
                }
            }

           container.model.SetData("childItemModel", childModel, i);
        }
    }

    function removeDependsFeature(packageId, featureId){
        console.log("FeaturesTreeView removeDependsFeature", packageId, featureId);
        if (!container.dependModel ||!container.dependModel.GetItemsCount() === 0){
            return;
        }

        for (var i = 0; i < container.dependModel.GetItemsCount(); i++){
            var rootFeatureId = container.dependModel.GetData("RootFeatureId", i);
            var rootPackageId = container.dependModel.GetData("RootPackageId", i);

            if (packageId === rootPackageId && rootFeatureId === featureId){
                console.log("FeaturesTreeView removeDependsFeature Removed", packageId, featureId);
                container.dependModel.RemoveItem(i);
                break;
            }

            var packagesModel = container.dependModel.GetData("Packages", i);

            for (var j = 0; j < packagesModel.GetItemsCount(); j++){
                var currentPackageId = packagesModel.GetData("Id", j);

                var currentChildModel = packagesModel.GetData("childItemModel", j);

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

                packagesModel.SetData("childItemModel", currentChildModel, j)

                if (currentChildModel.GetItemsCount() === 0){
                    packagesModel.RemoveItem(j);
                }
            }

            container.dependModel.SetData("Packages", packagesModel, i);

            if (packagesModel.GetItemsCount() === 0){
                container.dependModel.RemoveItem(i);
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
        var packageModel = container.dependModel.GetData("Packages", rootIndex);

        if (!packageModel) {
            return -1;
        }

        for (var i = 0; i < packageModel.GetItemsCount(); i++) {
            if (packageModel.GetData("Id", i) === packageId) {
                return i;
            }
        }

        return -1;
    }

    function addNewPackageToRootFeatureByRootIndex(packageId, rootIndex) {
        var packageModel = container.dependModel.GetData("Packages", rootIndex);
        var index = packageModel.InsertNewItem();
        packageModel.SetData("Id", packageId, index);
        container.dependModel.SetData("Packages", packageModel, rootIndex)
        return index;
    }

    function getIndexByRootFeatureId(featureId) {

        for (var i = 0; i < container.dependModel.GetItemsCount(); i++) {
            if (container.dependModel.GetData("RootFeatureId", i) === featureId) {
                return i;
            }
        }

        return -1;
    }

    function getIndexByPackageId(rootIndex, packageId) {

        var packages = container.dependModel.GetData("Packages", rootIndex);

        if (!packages) {
            return -1;
        }

        for (var i = 0; i < packages.GetItemsCount(); i++) {
            if (packages.GetData("Id", i) === packageId) {
                return i;
            }
        }

        return -1;
    }


    function featureIdHasDependency(featureId) {
        if (!container.dependModel) {
            return false;
        }

        for (var i = 0; i < container.dependModel.GetItemsCount(); i++) {
            if (container.dependModel.GetData("RootFeatureId", i) === featureId) {
                return true;
            }
        }

        return false;
    }

    function rootFeatureIdHasDependPackage(rootIndex) {
        var packageModel = container.dependModel.GetData("Packages", rootIndex);

        if (!packageModel) {
            return false;
        }
        return true;
    }

    function dependModelPackageHasChild(rootIndex, packageIndex) {
        var packageModel = container.dependModel.GetData("Packages", rootIndex);
        var childModel = packageModel.GetData("childItemModel", packageIndex);

        if (childModel && childModel.GetItemsCount() > 0) {
            return true;
        }
        return false;
    }

    function dependModelRemovePackageByIndex(rootIndex, packageIndex) {
        var packageModel = container.dependModel.GetData("Packages", rootIndex);
        packageModel.RemoveItem(packageIndex);
        container.dependModel.SetData("Packages", packageModel,  rootIndex);
    }

    function dependModelRemoveRootFeatureByIndex(rootIndex) {
        container.dependModel.RemoveItem(rootIndex);
    }

    function dependModePackageGetIndexByFeatureId(rootIndex, packageIndex, featureId) {
        var packageModel = container.dependModel.GetData("Packages", rootIndex);

        var childModel = packageModel.GetData("childItemModel", packageIndex);

        if (!packageModel || !childModel) {
            return -1;
        }

        for (var i = 0; i < childModel.GetItemsCount(); i++) {
            if (childModel.GetData("Id", i) === featureId) {
                return i;
            }
        }

        return -1;
    }

    function findInAllRootFeaturesDependFeatureById(packageId, featureId) {

        if (!container.dependModel) {
            return false;
        }

        var result = [];
        for (var i = 0; i < container.dependModel.GetItemsCount(); i++) {
            var packagesModel = container.dependModel.GetData("Packages", i);
            var rootFeatureId = container.dependModel.GetData("RootFeatureId", i);
            var rootPackageId = container.dependModel.GetData("RootPackageId", i);
            if (!packagesModel) {
                continue;
            }

            for (var j = 0; j < packagesModel.GetItemsCount(); j++) {
                var pId = packagesModel.GetData("Id", j);
                if (pId === packageId) {
                    var childModel = packagesModel.GetData("childItemModel", j);

                    if (!childModel) {
                        continue;
                    }

                    for (var k = 0; k < childModel.GetItemsCount(); k++) {
                        var fId = childModel.GetData("Id", k);

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
                        container.model = dataModelLocal;
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
                        container.dependModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
