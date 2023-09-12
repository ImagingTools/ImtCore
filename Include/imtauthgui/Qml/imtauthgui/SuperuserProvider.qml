import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: root;

    signal result(bool exists, string error);

    signal modelStateChanged(string state);

//    property GqlDocumentDataController documentController: GqlDocumentDataController {
//        onDocumentModelChanged: {
//            console.log("onDocumentModelChanged", documentModel.toJSON());
//            root.result(true, "");
//        }

//        onError: {
//            console.log("onError", message);
//            root.result(false, "");
//        }

//        onGetModelStateChanged: {
//            root.modelStateChanged(this.getModelState);
//        }
//    }

    function superuserExists(){
        console.log("superuserExists");

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

            console.log("UserItem query", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (root.gqlGetModel.ContainsKey("errors")){
                    dataModelLocal = root.gqlGetModel.GetData("errors");

//                    if (dataModelLocal.ContainsKey(root.getCommandId)){
//                        dataModelLocal = dataModelLocal.GetData(root.getCommandId);
//                    }

//                    let message = ""
//                    if (dataModelLocal.ContainsKey("message")){
//                        message = dataModelLocal.GetData("message");
//                    }

//                    let type;
//                    if (dataModelLocal.ContainsKey("type")){
//                        type = dataModelLocal.GetData("type");
//                    }

//                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                    root.result(false, "");

                    return;
                }

                if (root.gqlGetModel.ContainsKey("data")){
                    root.result(true, "");
                }
            }
            else if (this.state === "Error"){
//                Events.sendEvent("SendError", {"Message": message, "ErrorType": type})
            }
        }
    }//GqlModel itemModel
}


