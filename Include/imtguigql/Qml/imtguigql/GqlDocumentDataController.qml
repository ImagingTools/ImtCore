import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property string documentTypeId: "";
    property string updateCommandId: container.documentTypeId + "Update";
    property string getCommandId: container.documentTypeId + "Item";
    property string setCommandId: container.documentTypeId + "Add";

    property TreeItemModel documentModel: null;

    signal documentUpdated(string documentId, string documentName);
    signal documentAdded(string documentId, string documentName);
    signal savingError(string message);

    signal error(int type, string message);

    function getData(documentId, inputParams, typeId){
        if (typeId){
            container.documentTypeId = typeId;
        }

        container.gqlGetModel.getModelData(documentId, inputParams);
    }

    function setData(documentId, documentData){
        container.gqlSetModel.set(setCommandId, documentId, documentData);
    }

    function updateData(documentId, documentData){
        container.gqlSetModel.set(updateCommandId, documentId, documentData);
    }

    property string getModelState: gqlGetModel.state;
    property string setModelState: gqlSetModel.state;

    property GqlModel gqlGetModel: GqlModel {
        function getModelData(modelId, externInputParams){
            console.log("query container.gqlGetModel", container.getCommandId, modelId);
            var query = Gql.GqlRequest("query", container.getCommandId);

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

            console.log(container.getCommandId + "Item query", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, container.gqlGetModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (container.gqlGetModel.ContainsKey("errors")){
                    dataModelLocal = container.gqlGetModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(container.getCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.getCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

//                    container.error(message);

                    return;
                }

                dataModelLocal = container.gqlGetModel.GetData("data");
                if(dataModelLocal && dataModelLocal.ContainsKey(container.getCommandId)){
                    dataModelLocal = dataModelLocal.GetData(container.getCommandId);

                    container.documentModel = dataModelLocal;
                }
            }
            else if (this.state === "Error"){
                container.error("Unknown error");
            }
        }
    }//GqlModel itemModel

    property GqlModel gqlSetModel: GqlModel {
        function set(commandId, modelId, data){
            var query = Gql.GqlRequest("query", commandId);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", modelId);
            var jsonString = data.toJSON();
            inputParams.InsertField ("Item", jsonString);
            query.AddParam(inputParams);

            if (commandId === container.setCommandId){
                var queryFields = Gql.GqlObject("addedNotification");
                queryFields.InsertField("Id");
                query.AddField(queryFields);
            }
            else if (commandId === container.updateCommandId){
                var queryFields = Gql.GqlObject("updatedNotification");
                queryFields.InsertField("Id");
                query.AddField(queryFields);
            }

            var gqlData = query.GetQuery();

            console.log(gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, container.gqlSetModel);
            if (this.state === "Ready"){

                console.log("Data Ready");
                var dataModelLocal;
                if (container.gqlSetModel.ContainsKey("errors")){
                    dataModelLocal = container.gqlSetModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(container.setCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.setCommandId);
                    }
                    else if (dataModelLocal.ContainsKey(container.updateCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.updateCommandId);
                    }

//                    if (dataModelLocal.ContainsKey("message")){
//                        let message = dataModelLocal.GetData("message");
//                        container.savingError(message);
//                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                    return;
                }

                console.log("container.gqlSetModel", container.gqlSetModel.toJSON());

                if (container.gqlSetModel.ContainsKey("data")){
                    dataModelLocal = container.gqlSetModel.GetData("data");

                    if (dataModelLocal.ContainsKey(container.setCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.setCommandId);
                        dataModelLocal = dataModelLocal.GetData("addedNotification");

                        let documentId = dataModelLocal.GetData("Id");
                        let documentName = dataModelLocal.GetData("Name");
                        container.documentAdded(documentId, documentName);
                    }
                    else if (dataModelLocal.ContainsKey(container.updateCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.updateCommandId);
                        dataModelLocal = dataModelLocal.GetData("updatedNotification");

                        let documentId = dataModelLocal.GetData("Id");
                        let documentName = dataModelLocal.GetData("Name");

                        console.log("documentAdded", documentId, documentName);
                        container.documentUpdated(documentId, documentName);
                    }
                }
            }
        }
    }
}
