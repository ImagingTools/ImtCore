import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property string documentTypeId: "";
    property string updateCommandId: container.documentTypeId + "Update";
    property string getCommandId: container.documentTypeId + "Item";
    property string setCommandId: container.documentTypeId + "Add";

    property TreeItemModel documentModel: null;

    property alias gqlGetModel: getModel;
    property alias gqlSetModel: setModel;

    signal documentUpdated(string documentId, string documentName);
    signal documentAdded(string documentId, string documentName);
    signal savingError(string message);

    onDocumentTypeIdChanged: {
        console.log("onDocumentTypeIdChanged", documentTypeId);
    }

    function getData(documentId, inputParams){
        getModel.getModelData(documentId, inputParams);
    }

    function setData(documentId, documentData){
        setModel.set(setCommandId, documentId, documentData);
    }

    function updateData(documentId, documentData){
        setModel.set(updateCommandId, documentId, documentData);
    }

    GqlModel {
        id: getModel;

        function getModelData(modelId, externInputParams){
            console.log("query getModel", container.getCommandId, modelId);
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
            console.log("State:", this.state, getModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (getModel.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = getModel.GetData("data");
                if(dataModelLocal && dataModelLocal.ContainsKey(container.getCommandId)){
                    dataModelLocal = dataModelLocal.GetData(container.getCommandId);

                    container.documentModel = dataModelLocal;
                }
            }
        }
    }//GqlModel itemModel

    GqlModel {
        id: setModel;

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
            console.log("State:", this.state, setModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (setModel.ContainsKey("errors")){
                    dataModelLocal = setModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(container.setCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.setCommandId);
                    }
                    else if (dataModelLocal.ContainsKey(container.updateCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.updateCommandId);
                    }

                    if (dataModelLocal.ContainsKey("message")){
                        let message = dataModelLocal.GetData("message");
                        container.savingError(message);
                    }

                    return;
                }

                if (setModel.ContainsKey("data")){
                    dataModelLocal = setModel.GetData("data");

                    if (dataModelLocal.ContainsKey(setCommandId)){
                        dataModelLocal = dataModelLocal.GetData(setCommandId);
                        dataModelLocal = dataModelLocal.GetData("addedNotification");

                        let documentId = dataModelLocal.GetData("Id");
                        let documentName = dataModelLocal.GetData("Name");
                        container.documentUpdated(documentId, documentName);
                    }
                    else if (dataModelLocal.ContainsKey(updateCommandId)){
                        dataModelLocal = dataModelLocal.GetData(updateCommandId);
                        dataModelLocal = dataModelLocal.GetData("updatedNotification");

                        let documentId = dataModelLocal.GetData("Id");
                        let documentName = dataModelLocal.GetData("Name");
                        container.documentAdded(documentId, documentName);
                    }
                }
            }
        }
    }
}
