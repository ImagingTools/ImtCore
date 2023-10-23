import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: root;

    property string documentTypeId: "";
    property string documentId: "";

    property TreeItemModel historyModel: TreeItemModel {}

    property alias state: root.documentHistoryGqlModel.state;

    property GqlModel documentHistoryGqlModel: GqlModel {
        function getModel(documentId){
            var query = Gql.GqlRequest("query", root.documentTypeId + "History");

            var queryFields = Gql.GqlObject("itemHistory");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", documentId);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, root.documentHistoryGqlModel);
            if (this.state === "Ready"){
                if (root.documentHistoryGqlModel.ContainsKey("errors")){
                    return;
                }

                if (root.documentHistoryGqlModel.ContainsKey("data")){
                    var dataModelLocal = root.documentHistoryGqlModel.GetData("data");

                    if (dataModelLocal){
                        if (dataModelLocal.ContainsKey(root.documentTypeId + "History")){
                            dataModelLocal = dataModelLocal.GetData(root.documentTypeId + "History");
                            root.historyModel = dataModelLocal;
                        }
                    }
                }
            }
        }
    }

    function updateModel(documentType, documentId){
        root.documentTypeId = documentType;
        root.documentHistoryGqlModel.getModel(documentId);
    }
}
