import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0

QtObject {
    id: root;

    signal result(bool exists, string error);

    signal modelStateChanged(string state);

    function superuserExists(){
        root.gqlGetModel.getModelData("su", {});
    }

    property GqlModel gqlGetModel: GqlModel {
        function getModelData(modelId, externInputParams){
            var query = Gql.GqlRequest("query", "UserItem");

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", modelId);
            if (externInputParams){
                let keys = Object.keys(externInputParams)
                for (let key of keys){
                    inputParams.InsertField(key, externInputParams[key]);
                }
            }
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("Superuser model:", this.toJson());

            if (this.state === "Ready"){
                var dataModelLocal;

                if (root.gqlGetModel.containsKey("errors")){
                    dataModelLocal = root.gqlGetModel.getData("errors");

                    root.result(false, "");

                    return;
                }

                if (root.gqlGetModel.containsKey("data")){
                    root.result(true, "");
                }
            }
        }
    }//GqlModel itemModel
}


