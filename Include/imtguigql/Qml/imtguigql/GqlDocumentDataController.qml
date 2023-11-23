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

    signal result(var documentModel);

    function getData(documentId, inputParams, typeId, callback){
        if (typeId){
            container.documentTypeId = typeId;
        }
        console.log("getData", documentId, typeId, "container.documentTypeId ", container.documentTypeId)

        container.gqlGetModel.getModelData(documentId, inputParams);

        if (callback){
            container.result.connect(callback)
        }
    }

    function setData(documentId, documentData, additionInputParams, callback){
        container.gqlSetModel.set(setCommandId, documentId, documentData, additionInputParams);

        if (callback){
            container.documentAdded.connect(callback)
        }
    }

    function updateData(documentId, documentData, additionInputParams, callback){
        container.gqlSetModel.set(updateCommandId, documentId, documentData, additionInputParams);

        if (callback){
            container.documentUpdated.connect(callback)
        }
    }

    onDocumentTypeIdChanged: {
        console.log("onDocumentTypeIdChanged", container.documentTypeId)
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
//                    if (typeof externInputParams[key] === "object"){
                    if (key === "addition"){
                        let objectParams = externInputParams[key];
                        if (Object.keys(objectParams).length > 0){
                            let inputObject = Gql.GqlObject(key);
                            for (let keyObject in objectParams){
                                inputObject.InsertField(keyObject, objectParams[keyObject]);
                                console.log("_DEBUG_addition", keyObject, objectParams[keyObject])
                            }
                            inputParams.InsertFieldObject(inputObject);
                        }
                    }
                    else if (key !== "Parent"){
                        inputParams.InsertField(key, externInputParams[key]);
                    }
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

                    container.result(dataModelLocal);
                }
            }
            else if (this.state === "Error"){
               // Events.sendEvent("SendError", {"Message": "Unknown error", "ErrorType": "Critical"})
            }
        }
    }//GqlModel itemModel

    property GqlModel gqlSetModel: GqlModel {
        function set(commandId, modelId, data, additionInputParams){
            console.log("query container.gqlSetModel", container.getCommandId, modelId);
            var query = Gql.GqlRequest("mutation", commandId);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", modelId);
            var jsonString = data.toJSON();
            inputParams.InsertField ("Item", jsonString);
            if (additionInputParams){
                if (Object.keys(additionInputParams).length > 0){
                    let additionParams = Gql.GqlObject("addition");
                    for (let key in additionInputParams){
                        additionParams.InsertField(key, additionInputParams[key]);
                    }
                    inputParams.InsertFieldObject(additionParams);
                }
            }
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
