import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: provider;

    property TreeItemModel model: TreeItemModel {}

    function updateModel(){
        console.log("LicensesProvider updateModel");
        licensesModel.updateModel();
    }

    function getLicenseName(productId, licenseId){
        for (let i = 0; i < model.GetItemsCount(); i++){
            let id = model.GetData("Id", i);
            if (id === productId){
                let productLicensesModel = model.GetData("Licenses", i);
                 for (let licenseIndex = 0; licenseIndex < productLicensesModel.GetItemsCount(); licenseIndex++){
                     if (licenseId == productLicensesModel.GetData("Id", licenseIndex)){
                         return productLicensesModel.GetData("Name", licenseIndex)
                     }
                 }
            }
        }
        return ""
    }

    GqlModel {
        id: licensesModel;

        function updateModel() {
            console.log("LicensesProvider updateModel Licenses");
            var query = Gql.GqlRequest("query", "Licenses");

            var queryFields = Gql.GqlObject("LicensesItems");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State Licenses:", this.state);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("Licenses")){
                    dataModelLocal = dataModelLocal.GetData("Licenses");
                    provider.model = dataModelLocal;
                }
            }
        }
    }
}
