import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: commandsProviderContainer;

    property TreeItemModel commandsModel: TreeItemModel {};

    property string commandsId;
    property string documentUuid;

    signal modelLoaded();
    signal commandModeChanged(string commandId, bool newMode);

    Component.onDestruction: {
        Events.unSubscribeEvent(commandsProviderContainer.documentUuid + "_CommandsUpdateModel", commandsProviderContainer.updateModel)
    }

    onCommandsIdChanged: {
        commandsProviderContainer.modelCommands.updateModel();
    }

    onCommandsModelChanged: {
        commandsProviderContainer.updateGui();

        commandsProviderContainer.modelLoaded();
    }

    onDocumentUuidChanged: {
        Events.subscribeEvent(commandsProviderContainer.documentUuid + "_CommandsUpdateModel", commandsProviderContainer.updateModel)
    }

    onCommandModeChanged: {
        Events.sendEvent("CommandsEnabledChanged", {"CommandsId":commandsProviderContainer.documentUuid, "CommandId":commandId, "Enabled":newMode, "Model": commandsProviderContainer.commandsModel});
    }

    function updateModel(){
        commandsProviderContainer.modelCommands.updateModel();
    }

    function updateGui(){
        Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel,
                             "CommandsId": commandsProviderContainer.documentUuid});
    }

    function setCommandIsEnabled(commandId, isEnabled){
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

    property GqlModel modelCommands: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", commandsProviderContainer.commandsId + "Commands");
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

                    if(dataModelLocal.ContainsKey(commandsProviderContainer.commandsId + "Commands")){
                        dataModelLocal = dataModelLocal.GetData(commandsProviderContainer.commandsId + "Commands");

                        commandsProviderContainer.commandsModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
