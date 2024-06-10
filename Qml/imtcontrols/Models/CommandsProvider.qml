import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: commandsProviderContainer;

    property TreeItemModel commandsModel: null;

    property string uuid;

    signal commandModeChanged(string commandId, bool newMode);

    Component.onDestruction: {
        if (commandsModel){
            commandsModel.clear();
        }
    }

    onCommandModeChanged: {
        Events.sendEvent("CommandsEnabledChanged", {"Model": commandsModel, "CommandId": uuid});
    }

    onCommandsModelChanged: {
        Events.sendEvent("CommandsModelChanged", {"Model": commandsProviderContainer.commandsModel, "CommandId": commandsProviderContainer.uuid});
    }

    function commandIsEnabled(commandId){
        if(!commandsProviderContainer.commandsModel){
            return false;
        }

        for (let i = 0; i < commandsProviderContainer.commandsModel.getItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.getData("Id", i);
            let isEnabled = commandsProviderContainer.commandsModel.getData("IsEnabled", i);

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

        for (let i = 0; i < commandsProviderContainer.commandsModel.getItemsCount(); i++){
            let currentCommandId = commandsProviderContainer.commandsModel.getData("Id", i);
            if (currentCommandId === commandId){
                return i;
            }
        }

        return -1;
    }

    function mergeModelWith(externModel){
        for (let i = 0; i < externModel.getItemsCount(); i++){

            let index = commandsModel.insertNewItem()
            commandsModel.copyItemDataFromModel(index, externModel, i)
        }
    }

    function setAdditionalProperties(model){
        if (!model){
            return;
        }

        for (let i = 0; i < model.getItemsCount(); i++){
            model.setData("IsToggleable", false, i);
            model.setData("IsToggled", false, i);
        }
    }
}

