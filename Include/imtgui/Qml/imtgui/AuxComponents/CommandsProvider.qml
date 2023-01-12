import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: commandsProviderContainer;

    property string commandsId;

    property TreeItemModel commandsModel: TreeItemModel {};

    property bool localData: false; //The commands model is not loaded from the server

    signal modelLoaded();
    signal commandModeChanged(string commandId, string newMode);

    onCommandsIdChanged: {
        console.log("commandsProviderContainer onCommandsIdChanged", commandsProviderContainer.commandsId);
        modelCommands.updateModel();
    }

    onCommandsModelChanged: {
        Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel,
                                                  "CommandsId": commandsProviderContainer.commandsId});

        commandsProviderContainer.modelLoaded();
    }

    function setCommandIsEnabled(commandId, isEnabled){
        if(commandsModel === undefined) return;

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId == commandId){
                let currentIsEnabled = commandsProviderContainer.commandsModel.GetData("IsEnabled", i);
                if (currentIsEnabled != isEnabled){
                    commandsProviderContainer.commandsModel.SetData("IsEnabled", isEnabled, i);

                    commandModeChanged(commandId, isEnabled);
                }
            }
        }
    }

    function commandIsEnabled(commandId){
        if(commandsModel === undefined) return;

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            let isEnabled = commandsProviderContainer.commandsModel.GetData("IsEnabled", i);

            if (currentCommandId == commandId){
                return isEnabled;
            }
        }

        return false;
    }

    function mergeModelWith(externModel){
        for (let i = 0; i < externModel.GetItemsCount(); i++){

            let index = commandsModel.InsertNewItem()
            commandsModel.CopyItemDataFromModel(index, externModel, i)
        }
    }

    GqlModel {
        id: modelCommands;

        function updateModel() {
            console.log("CommandsProvider updateModel", commandsProviderContainer.commandsId + "Commands");
            var query = Gql.GqlRequest("query", commandsProviderContainer.commandsId + "Commands");

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
                    if(dataModelLocal.ContainsKey(commandsProviderContainer.commandsId + "Commands")){
                        dataModelLocal = dataModelLocal.GetData(commandsProviderContainer.commandsId + "Commands");

                        commandsProviderContainer.commandsModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
