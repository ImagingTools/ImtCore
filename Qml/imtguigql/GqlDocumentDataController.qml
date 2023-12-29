import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property string getModelState: gqlGetModel.state;
    property string setModelState: gqlSetModel.state;
    property string updateModelState: gqlUpdateModel.state;

    signal error(string message, string type);

    function getData(documentTypeId, documentId, additionInputParams, callback){
        if (documentTypeId === ""){
            console.error("Get data object was failed! DocumentTypeId is invalid.");
            return;
        }

        if (!additionInputParams){
            additionInputParams = {}
        }

        container.gqlGetModel.getData(documentTypeId, documentId, additionInputParams, callback);
    }

    function setData(addCommandId, documentId, documentData, additionInputParams, callback){
        console.log("setData", addCommandId, documentId, documentData, callback);

        if (addCommandId === ""){
            console.error("Set data object was failed! addCommandId is invalid.");
            return;
        }

        if (!additionInputParams){
            additionInputParams = {}
        }

        container.gqlSetModel.setData(addCommandId, documentId, documentData, additionInputParams, callback);
    }

    function updateData(documentTypeId, documentId, documentData, additionInputParams, callback){
        if (documentTypeId === ""){
            console.error("Update data object was failed! DocumentTypeId is invalid.");
            return;
        }

        if (!additionInputParams){
            additionInputParams = {}
        }

        container.gqlUpdateModel.setData(documentTypeId, documentId, documentData,additionInputParams, callback);
    }

    property GqlModel gqlGetModel: GqlModel {
        function getData(documentTypeId, documentId, additionInputParams, callback){
            var query = Gql.GqlRequest("query", documentTypeId + "Item");

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", documentId);

            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);

            let onResult = function(){
                let state = container.gqlGetModel.state;
                console.log("onResult", state);

                if (state === "Error"){
                    container.gqlGetModel.onStateChanged.disconnect(onResult);

                    container.error("Network error", "Critical");
                }
                else if (state === "Ready"){
                    if (container.gqlGetModel.ContainsKey("errors")){
                        let dataModelLocal = container.gqlGetModel.GetData("errors");

                        if (dataModelLocal.ContainsKey(documentTypeId + "Item")){
                            dataModelLocal = dataModelLocal.GetData(documentTypeId + "Item");
                        }

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        container.error(message, type);
                    }

                    else if (container.gqlGetModel.ContainsKey("data")){
                        let dataModelLocal = container.gqlGetModel.GetData("data")

                        if (dataModelLocal.ContainsKey(documentTypeId + "Item")){
                            dataModelLocal = dataModelLocal.GetData(documentTypeId + "Item")

                            if (callback){
                                callback(dataModelLocal)
                            }
                        }
                    }

                    container.gqlGetModel.onStateChanged.disconnect(onResult);
                }
            }

            this.onStateChanged.connect(onResult);
        }
    }//GqlModel itemModel

    property GqlModel gqlSetModel: GqlModel {
        function setData(addCommandId, documentId, documentData, additionInputParams, callback){

            console.log("gqlSetModel additionInputParams", JSON.stringify(additionInputParams));
            var query = Gql.GqlRequest("mutation", addCommandId);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", documentId);
            var jsonString = documentData.toJSON();
            inputParams.InsertField ("Item", jsonString);

            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("addedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);

            let onResult = function(){
                let state = container.gqlSetModel.state;
                console.log("onResult", state);

                if (state === "Error"){
                    container.gqlSetModel.onStateChanged.disconnect(onResult);

                    container.error("Network error", "Critical");
                }
                if (state === "Ready"){
                    var dataModelLocal;
                    if (container.gqlSetModel.ContainsKey("errors")){
                        dataModelLocal = container.gqlSetModel.GetData("errors");

                        if (dataModelLocal.ContainsKeya(addCommandId)){
                            dataModelLocal = dataModelLocal.GetData(addCommandId);
                        }

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        container.error(message, type);
                    }

                    else if (container.gqlSetModel.ContainsKey("data")){
                        dataModelLocal = container.gqlSetModel.GetData("data");

                        if (dataModelLocal.ContainsKey(addCommandId)){
                            dataModelLocal = dataModelLocal.GetData(addCommandId);
                            dataModelLocal = dataModelLocal.GetData("addedNotification");

                            let documentId = dataModelLocal.GetData("Id");
                            let documentName = dataModelLocal.GetData("Name");

                            if (callback){
                                callback(documentId, documentName);
                            }
                        }
                    }

                    container.gqlSetModel.onStateChanged.disconnect(onResult);
                }
            }

            this.onStateChanged.connect(onResult);
        }
    }

    property GqlModel gqlUpdateModel: GqlModel {
        function setData(documentTypeId, documentId, documentData, additionInputParams, callback){
            var query = Gql.GqlRequest("mutation", documentTypeId + "Update");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", documentId);
            var jsonString = documentData.toJSON();
            inputParams.InsertField ("Item", jsonString);

            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }

                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("updatedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);

            let onResult = function(){
                let state = container.gqlUpdateModel.state;
                console.log("onResult", state);

                if (state === "Error"){
                    container.gqlUpdateModel.onStateChanged.disconnect(onResult);

                    container.error("Network error", "Critical");
                }
                if (state === "Ready"){
                    var dataModelLocal;
                    if (container.gqlUpdateModel.ContainsKey("errors")){
                        dataModelLocal = container.gqlUpdateModel.GetData("errors");

                        if (dataModelLocal.ContainsKey(documentTypeId + "Update")){
                            dataModelLocal = dataModelLocal.GetData(documentTypeId + "Update");
                        }

                        let message = ""
                        if (dataModelLocal.ContainsKey("message")){
                            message = dataModelLocal.GetData("message");
                        }

                        let type;
                        if (dataModelLocal.ContainsKey("type")){
                            type = dataModelLocal.GetData("type");
                        }

                        container.error(message, type);
                    }

                    else if (container.gqlUpdateModel.ContainsKey("data")){
                        dataModelLocal = container.gqlUpdateModel.GetData("data");

                        if (dataModelLocal.ContainsKey(documentTypeId + "Update")){
                            dataModelLocal = dataModelLocal.GetData(documentTypeId + "Update");
                            dataModelLocal = dataModelLocal.GetData("updatedNotification");

                            let documentId = dataModelLocal.GetData("Id");
                            let documentName = dataModelLocal.GetData("Name");

                            if (callback){
                                callback(documentId, documentName);
                            }
                        }
                    }

                    container.gqlUpdateModel.onStateChanged.disconnect(onResult);
                }
            }

            this.onStateChanged.connect(onResult);
        }
    }
}
