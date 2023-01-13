import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property string prefixCommandId: "";

    property string getCommandId: prefixCommandId + "Item";
    property string setCommandId: prefixCommandId + "Add";
    property string updateCommandId: prefixCommandId + "Update";

    property TreeItemModel documentModel: TreeItemModel {}

    function getData(documentId, inputParams){
        getModel.get(documentId, inputParams);
    }

    function setData(documentId, documentData){
        setModel.set(setCommandId, documentId, documentData);
    }

    function updateData(documentId, documentData){
        setModel.set(updateCommandId, documentId, documentData);
    }

    GqlModel {
        id: getModel;

        function get(modelId, externInputParams){
            var query = Gql.GqlRequest("query", container.getCommandId);

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var inputParams = Gql.GqlObject("input");
            let keys = Object.keys(externInputParams)
            for (let key of keys){
                inputParams.InsertField(key, externInputParams[key]);
            }
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, getModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (getModel.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = itemModel.GetData("data");
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
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            inputParams.InsertField ("Item", jsonString);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("New");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, setModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (setModel.ContainsKey("errors")){
                    dataModelLocal = setModel.GetData("errors");
                    dataModelLocal = dataModelLocal.GetData(container.setCommandId);
                    if (dataModelLocal.ContainsKey("message")){
                        let message = dataModelLocal.GetData("message");
                        modalDialogManager.openDialog(errorDialog, {"message": message});
                    }

                    return;
                }

                if (setModel.ContainsKey("data")){
                    dataModelLocal = setModel.GetData("data");
                }
            }
        }
    }
}
