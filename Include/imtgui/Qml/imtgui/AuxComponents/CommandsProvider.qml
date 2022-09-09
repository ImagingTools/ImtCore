import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: commandsProviderContainer;

    property string commandsId;

    property TreeItemModel commandsModel;

    onCommandsIdChanged: {
        console.log("commandsProviderContainer onCommandsIdChanged", commandsProviderContainer.commandsId);
        modelCommands.updateModel();
    }

    function changeCommandMode(commandId, mode){
        console.log("CommandsController changeCommandMode");

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let m_commandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (m_commandId == commandId){
                commandsProviderContainer.commandsModel.SetData("Mode", mode, i);
            }
        }
    }

    function getCommandMode(commandId){
        console.log("CommandsController getCommandMode", commandId);

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let m_commandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            let m_mode = commandsProviderContainer.commandsModel.GetData("Mode", i);

            if (m_commandId == commandId){
                return m_mode;
            }
        }

        return null;
    }

    GqlModel {
        id: modelCommands;

        function updateModel() {
            console.log("modelCommands topPanel.activeCommandsModelId");
            var query = Gql.GqlRequest("query", "CommandsData");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("PageId", commandsProviderContainer.commandsId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            queryFields.InsertField("Icon");
            queryFields.InsertField("Mode");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }


        onStateChanged: {
            console.log("State:", this.state, modelCommands);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (modelCommands.ContainsKey("errors")){
                    return;
                }

                if (modelCommands.ContainsKey("data")){
                    dataModelLocal = modelCommands.GetData("data")
                    if(dataModelLocal.ContainsKey("CommandsData")){
                        dataModelLocal = dataModelLocal.GetData("CommandsData");

                        if(dataModelLocal.ContainsKey("items")){
                            dataModelLocal = dataModelLocal.GetData("items");
                            commandsProviderContainer.commandsModel = dataModelLocal;
                            Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel,
                                                                      "CommandsId": commandsProviderContainer.commandsId});
                        }
                    }
                }
            }
        }
    }

}
