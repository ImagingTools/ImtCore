import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: root;

    property string commandId: "GetOrderHistory";

    property TreeItemModel historyModel: TreeItemModel {}

    property GqlModel documentHistoryGqlModel: GqlModel {
        function getModel(documentId){
            var query = Gql.GqlRequest("query", root.commandId);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", documentId);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            console.log("query", gqlData);
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
                        if (dataModelLocal.ContainsKey(root.commandId)){
                            dataModelLocal = dataModelLocal.GetData(root.commandId);
                            root.historyModel = dataModelLocal;
                        }
                    }
                }
            }
        }
    }

    function updateModel(documentId){
        root.documentHistoryGqlModel.getModel(documentId);
    }
}
