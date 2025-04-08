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
        pagesModel.clear();
    }

    property string modelState: pagesProvider.pagesGqlModel.state;

    property GqlModel pagesGqlModel : GqlModel {
        function updateModel(){
            var query = Gql.GqlRequest("query", "PagesData");

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("pageId");
            queryFields.InsertField("name");
            queryFields.InsertField("icon");
            queryFields.InsertField("source");
            queryFields.InsertField("startItem");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData)
        }

        onStateChanged: {
            if (this.state === "Ready"){
                if (this.containsKey("data")){
                    var dataModelLocal = this.getData("data");

                    if(dataModelLocal.containsKey("PagesData")){
                        dataModelLocal = dataModelLocal.getData("PagesData");

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
