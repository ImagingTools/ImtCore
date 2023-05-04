import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: provider;

    property TreeItemModel model: TreeItemModel {}

    property bool completed: false;

    function updateModel(){
        console.log("LicensesProvider updateModel");
        licensesModel.updateModel();
    }

    function getLicenseName(productId, licenseId){
        for (let i = 0; i < provider.model.GetItemsCount(); i++){
            let id = provider.model.GetData("Id", i);
            if (id === productId){
                if (provider.model.ContainsKey("Licenses", i)){
                    let productLicensesModel = provider.model.GetData("Licenses", i);
                     for (let licenseIndex = 0; licenseIndex < productLicensesModel.GetItemsCount(); licenseIndex++){
                         if (licenseId === productLicensesModel.GetData("Id", licenseIndex)){
                             return productLicensesModel.GetData("Name", licenseIndex)
                         }
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

            provider.completed = false;;

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State Licenses:", this.state);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if (dataModelLocal.ContainsKey("Licenses")){
                    dataModelLocal = dataModelLocal.GetData("Licenses");
                    provider.model = dataModelLocal;

                    provider.completed = true;
                }
            }
        }
    }
}
