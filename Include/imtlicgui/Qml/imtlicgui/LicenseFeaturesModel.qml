import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: licenseFeaturesModelContainer;

    property TreeItemModel modelLicenseFeatures;

    Component.onCompleted: {
        Events.subscribeEvent("LicenseFeaturesUpdate", updateModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("LicenseFeaturesUpdate", updateModel);
    }

    onModelLicenseFeaturesChanged: {
        console.log("licenseFeaturesModelContainer onModelLicenseFeaturesChanged");
        Events.sendEvent("LicenseFeaturesModelUpdated");
    }

    function updateModel(){
        licensesDependenciesModel.updateModel();
    }

    function updateLicensesDependenciesAfterLicenseEditing(licenseOldId, licenseNewId){
        console.log("updateLicensesDependenciesAfterLicenseEditing", licenseOldId, licenseNewId);

        if (!licenseFeaturesModelContainer.modelLicenseFeatures){
            return;
        }

        console.log("1", licenseFeaturesModelContainer.modelLicenseFeatures.toJSON());
        let keys = licenseFeaturesModelContainer.modelLicenseFeatures.GetKeys();
        for (let i = 0; i < keys.length; i++){
            let value = licenseFeaturesModelContainer.modelLicenseFeatures.GetData(keys[i]);

            if (keys[i] === licenseOldId){
                licenseFeaturesModelContainer.modelLicenseFeatures.SetData(licenseNewId, value);
                licenseFeaturesModelContainer.modelLicenseFeatures.SetData(keys[i], "");
                break;
            }
        }
    }

    GqlModel {
        id: licensesDependenciesModel;

        function updateModel() {
            console.log( "FeaturesTreeView GqlModel loadLicenseDependModel");
            var query = Gql.GqlRequest("query", "LicenseFeatures");

            var queryFields = Gql.GqlObject("Features");
            queryFields.InsertField("TreeModel");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("TreeView loadLicenseDependModel query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, licensesDependenciesModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");

                if (dataModelLocal.ContainsKey("LicenseFeatures")){
                    dataModelLocal = dataModelLocal.GetData("LicenseFeatures");

                    if (dataModelLocal.ContainsKey("TreeModel")){
                        dataModelLocal = dataModelLocal.GetData("TreeModel");

                        licenseFeaturesModelContainer.modelLicenseFeatures = dataModelLocal;
                    }
                    else{
                        let treeModel = dataModelLocal.AddTreeModel("TreeModel");
                        licenseFeaturesModelContainer.modelLicenseFeatures =treeModel;
                    }
                }
            }
        }
    }
}
