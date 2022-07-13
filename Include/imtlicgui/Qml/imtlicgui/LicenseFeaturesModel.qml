import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: licenseFeaturesModelContainer;

    property TreeItemModel modelLicenseFeatures;

    onModelLicenseFeaturesChanged: {
        console.log("licenseFeaturesModelContainer onModelLicenseFeaturesChanged");
        Events.sendEvent("LicenseFeaturesModelUpdated");
    }

    function reloadModel(){
        licensesDependenciesModel.loadLicenseDependModel();
    }

    function updateLicensesDependenciesAfterLicenseEditing(productId, licenseOldId, licenseNewId, licenseNewName){
        console.log("updateLicensesDependenciesAfterLicenseEditing", productId, licenseOldId, licenseNewId, licenseNewName);

        if (!licenseFeaturesModelContainer.modelLicenseFeatures){
            return;
        }

        console.log("1", licenseFeaturesModelContainer.modelLicenseFeatures.toJSON());
        let keys = licenseFeaturesModelContainer.modelLicenseFeatures.GetKeys();
        for (let i = 0; i < keys.length; i++){
            let value = licenseFeaturesModelContainer.modelLicenseFeatures.GetData(keys[i]);

            if (keys[i] == licenseOldId){
                licenseFeaturesModelContainer.modelLicenseFeatures.SetData(licenseNewId, value);
                licenseFeaturesModelContainer.modelLicenseFeatures.SetData(keys[i], "");
                break;
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
            console.log("State:", this.state, licensesDependenciesModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");

                if (dataModelLocal.ContainsKey("LicensesDependencies")){
                    dataModelLocal = dataModelLocal.GetData("LicensesDependencies");

                    if (dataModelLocal.ContainsKey("TreeModel")){
                        dataModelLocal = dataModelLocal.GetData("TreeModel");
                    }

                    licenseFeaturesModelContainer.modelLicenseFeatures = dataModelLocal;
                }
            }
        }
    }
}
