import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: pagesProvider;

    property TreeItemModel pagesModel: TreeItemModel {};

    function updateModel(){
        pagesProvider.pagesGqlModel.updateModel();
    }

    function clearModel(){
        pagesModel.Clear();
    }

    property string modelState: pagesProvider.pagesGqlModel.state;

    property GqlModel pagesGqlModel : GqlModel {
        function updateModel(){
            var query = Gql.GqlRequest("query", "PagesData");

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("PageId");
            queryFields.InsertField("Name");
            queryFields.InsertField("Icon");
            queryFields.InsertField("Source");
            queryFields.InsertField("StartItem");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData)
        }

        onStateChanged: {
            if (this.state === "Ready"){
                if (this.ContainsKey("data")){
                    var dataModelLocal = this.GetData("data");

                    if(dataModelLocal.ContainsKey("PagesData")){
                        dataModelLocal = dataModelLocal.GetData("PagesData");

                        pagesProvider.pagesModel = dataModelLocal;
                    }
                }
            }
            else if (this.state === "Error"){
                ModalDialogManager.showWarningDialog("Request failed")
            }
        }
    }
}
