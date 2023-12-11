import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: commandsProviderContainer;

    property TreeItemModel commandsModel: null;

    property string commandId;
    property string uuid;

    property var additionInputParams: ({})

    signal commandModeChanged(string commandId, bool newMode);

    Component.onDestruction: {
        if (commandsModel){
            commandsModel.Clear();
        }
    }

    function updateModel(){
        if (commandId === ""){
            console.error("Unable to update commands model. Command-ID is empty.");
            return;
        }

        commandsProviderContainer.modelCommands.updateModel();
    }

    function updateGui(){
        console.log("CommandsProvider updateGui", commandId);
        Events.sendEvent("CommandsModelChanged", {"Model": commandsModel, "CommandId": uuid});
    }

    function clearGui(){
        Events.sendEvent("CommandsClearModel", {"CommandId": uuid});
    }

    function setCommandIsEnabled(commandId, isEnabled){
        if(!commandsProviderContainer.commandsModel){
            return;
        }

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
        if(!commandsProviderContainer.commandsModel){
            return;
        }

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                commandsProviderContainer.commandsModel.SetData("Icon", icon, i);
                break;
            }
        }
    }

    function setCommandName(commandId, commandName){
        if(!commandsProviderContainer.commandsModel){
            return;
        }

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                commandsProviderContainer.commandsModel.SetData("Name", commandName, i);
                break;
            }
        }
    }

    function setCommandNotification(commandId, notification){
        if(!commandsProviderContainer.commandsModel){
            return;
        }

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                commandsProviderContainer.commandsModel.SetData("Status", notification, i);
                break;
            }
        }
    }

    function commandIsEnabled(commandId){
        if(!commandsProviderContainer.commandsModel){
            return false;
        }

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
        let index = commandsProviderContainer.getCommandIndex(commandId);

        return index >= 0;
    }

    function getCommandIndex(commandId){
        if(!commandsProviderContainer.commandsModel){
            return -1;
        }

        for (let i = 0; i < commandsProviderContainer.commandsModel.GetItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                return i;
            }
        }

        return -1;
    }

    function mergeModelWith(externModel){
        for (let i = 0; i < externModel.GetItemsCount(); i++){

            let index = commandsModel.InsertNewItem()
            commandsModel.CopyItemDataFromModel(index, externModel, i)
        }
    }

    function setAdditionalProperties(model){
        if (!model){
            return;
        }

        for (let i = 0; i < model.GetItemsCount(); i++){
            model.SetData("IsToggleable", false, i);
            model.SetData("IsToggled", false, i);
        }
    }

    property GqlModel modelCommands: GqlModel {
        function updateModel() {
            console.log("__DEBUG__modelCommands", commandsProviderContainer.commandId, JSON.stringify(commandsProviderContainer.additionInputParams))

            var query = Gql.GqlRequest("query", commandsProviderContainer.commandId + "Commands");
            if (Object.keys(commandsProviderContainer.additionInputParams).length > 0){
                let inputParams = Gql.GqlObject("input");
                let additionParams = Gql.GqlObject("addition");
                for (let key in commandsProviderContainer.additionInputParams){
                    additionParams.InsertField(key, commandsProviderContainer.additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
                query.AddParam(inputParams);
            }
            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, commandsProviderContainer.modelCommands);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (commandsProviderContainer.modelCommands.ContainsKey("errors")){
                    return;
                }

                if (commandsProviderContainer.modelCommands.ContainsKey("data")){
                    dataModelLocal = commandsProviderContainer.modelCommands.GetData("data")
                    if (!dataModelLocal){
                        return;
                    }

                    if(dataModelLocal.ContainsKey(commandsProviderContainer.commandId + "Commands")){
                        dataModelLocal = dataModelLocal.GetData(commandsProviderContainer.commandId + "Commands");

                        commandsProviderContainer.setAdditionalProperties(dataModelLocal);

                        commandsProviderContainer.commandsModel = dataModelLocal;
                    }
                }
            }
        }
    }
}

