import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

CommandsController {
    id: commandsProviderContainer;

    property string commandId;

    onCommandIdChanged: {
        updateModel();
    }

    function updateModel(){
        if (commandId === ""){
            console.error("Unable to update commands model. Command-ID is empty.");
            return;
        }

        commandsProviderContainer.modelCommands.updateModel();
    }

    function getAdditionalInputParams(){
        return {};
    }

    property GqlModel modelCommands: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", commandsProviderContainer.commandId + "Commands");
            let additionInputParams = commandsProviderContainer.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let inputParams = Gql.GqlObject("input");
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
                query.AddParam(inputParams);
            }
            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);

            commandsProviderContainer.isReady = false;
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (this.ContainsKey("errors")){
                    return;
                }

                if (this.ContainsKey("data")){
                    dataModelLocal = this.GetData("data")
                    if (!dataModelLocal){
                        return;
                    }

                    if(dataModelLocal.ContainsKey(commandsProviderContainer.commandId + "Commands")){
                        dataModelLocal = dataModelLocal.GetData(commandsProviderContainer.commandId + "Commands");
                        if (dataModelLocal != "null"){
                            commandsProviderContainer.setAdditionalProperties(dataModelLocal);
                            commandsProviderContainer.commandsModel = dataModelLocal;

                            commandsProviderContainer.isReady = true;
                        }
                    }
                }
            }
        }
    }
}

