import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0

QtObject {
    id: provider;

    property TreeItemModel model: TreeItemModel {}

    property bool completed: false;

    signal failed();

    function updateModel(){
        provider.licensesModel.updateModel();
    }

    function getLicenseName(productId, licenseId){
        for (let i = 0; i < provider.model.getItemsCount(); i++){
            let id = provider.model.getData("id", i);
            if (id === productId){
                if (provider.model.containsKey("Licenses", i)){
                    let productLicensesModel = provider.model.getData("Licenses", i);
                     for (let licenseIndex = 0; licenseIndex < productLicensesModel.getItemsCount(); licenseIndex++){
                         if (licenseId === productLicensesModel.getData("id", licenseIndex)){
                             return productLicensesModel.getData("name", licenseIndex)
                         }
                     }
                }
            }
        }
        return ""
    }

    property GqlModel licensesModel: GqlModel {
        function updateModel(){
            var query = Gql.GqlRequest("query", "Licenses");

            var queryFields = Gql.GqlObject("LicensesItems");
            queryFields.InsertField("id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            provider.completed = false;;

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                if (provider.licensesModel.containsKey("data")){
                    var dataModelLocal = provider.licensesModel.getData("data");
                    if (dataModelLocal.containsKey("Licenses")){
                        dataModelLocal = dataModelLocal.getData("Licenses");
                        provider.model = dataModelLocal;

                        provider.completed = true;
                    }
                }
            }
            else if (this.state === "Error"){
                provider.failed();

                provider.completed = true;
            }
        }
    }
}
