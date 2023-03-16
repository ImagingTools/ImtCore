import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: commandsProviderContainer;

    property string commandsId;

    property string documentUuid;

    property TreeItemModel commandsModel: TreeItemModel {};

    signal modelLoaded();
    signal commandModeChanged(string commandId, bool newMode);

    onCommandsIdChanged: {
        console.log("commandsProviderContainer onCommandsIdChanged", commandsProviderContainer.commandsId);
        modelCommands.updateModel();
    }

    onCommandsModelChanged: {
        Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel,
                                                  "CommandsId": commandsProviderContainer.documentUuid});

        commandsProviderContainer.modelLoaded();
    }

    function updateModel(){
//        Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel,
//                                                  "CommandsId": commandsProviderContainer.commandsId});

        modelCommands.updateModel();
    }

    function updateGui(){
        Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel,
                             "CommandsId": commandsProviderContainer.documentUuid});
    }

    function setCommandIsEnabled(commandId, isEnabled){
        console.log("commandsProviderContainer setCommandIsEnabled", commandId, isEnabled);

        if(commandsProviderContainer.commandsModel === undefined) return;

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId == commandId){
                let currentIsEnabled = commandsProviderContainer.commandsModel.GetData("IsEnabled", i);
                if (currentIsEnabled !== isEnabled){
                    console.log("SetData setCommandIsEnabled", commandId, isEnabled);
                    commandsProviderContainer.commandsModel.SetData("IsEnabled", isEnabled, i);

                    commandsProviderContainer.commandModeChanged(commandId, isEnabled);
                }
            }
        }
    }

    function setCommandIcon(commandId, icon){
        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                commandsProviderContainer.commandsModel.SetData("Icon", icon, i);
                break;
            }
        }
    }

    function setCommandName(commandId, commandName){
        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                commandsProviderContainer.commandsModel.SetData("Name", commandName, i);
                break;
            }
        }
    }

    function setCommandNotification(commandId, notification){
        console.log("setCommandNotification", commandId, notification);
        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                commandsProviderContainer.commandsModel.SetData("Status", notification, i);
                break;
            }
        }
    }

    function commandIsEnabled(commandId){
        if(commandsProviderContainer.commandsModel === undefined) return;

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            let isEnabled = commandsProviderContainer.commandsModel.GetData("IsEnabled", i);

            if (currentCommandId == commandId){
                return isEnabled;
            }
        }

        return false;
    }

    function commandExists(commandId){
        if(commandsProviderContainer.commandsModel === undefined) return false;

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                return true;
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
