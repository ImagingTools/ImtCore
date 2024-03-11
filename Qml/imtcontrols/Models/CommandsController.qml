import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root;

    property TreeItemModel commandsModel: TreeItemModel {}

    property string uuid;

    signal commandModeChanged(string commandId, bool newMode);

    onCommandModeChanged: {
        Events.sendEvent("CommandsEnabledChanged", {"Model": root.commandsModel, "CommandId": uuid});
    }

    function commandIsEnabled(commandId){
        if(!root.commandsModel){
            return false;
        }

        for (let i = 0; i < root.commandsModel.GetItemsCount(); i++){
            let currentCommandId = root.commandsModel.GetData("Id", i);
            let isEnabled = root.commandsModel.GetData("IsEnabled", i);

            if (currentCommandId == commandId){
                return isEnabled;
            }
        }

        return false;
    }

    function setCommandIsEnabled(commandId, isEnabled){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.GetItemsCount(); i++){
            let currentCommandId = root.commandsModel.GetData("Id", i);
            if (currentCommandId == commandId){
                let currentIsEnabled = root.commandsModel.GetData("IsEnabled", i);
                if (currentIsEnabled !== isEnabled){
                    root.commandsModel.SetData("IsEnabled", isEnabled, i);
                }
            }
        }
    }

    function setCommandIcon(commandId, icon){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.GetItemsCount(); i++){
            let currentCommandId = root.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.SetData("Icon", icon, i);
                break;
            }
        }
    }

    function setCommandName(commandId, commandName){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.GetItemsCount(); i++){
            let currentCommandId = root.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.SetData("Name", commandName, i);
                break;
            }
        }
    }

    function setCommandNotification(commandId, notification){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.GetItemsCount(); i++){
            let currentCommandId = root.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.SetData("Status", notification, i);
                break;
            }
        }
    }

    function getCommandIndex(commandId){
        if(!root.commandsModel){
            return -1;
        }

        for (let i = 0; i < root.commandsModel.GetItemsCount(); i++){
            let currentCommandId = root.commandsModel.GetData("Id", i);
            if (currentCommandId === commandId){
                return i;
            }
        }

        return -1;
    }

    function setIsToggleable(commandId, toggleable){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            root.commandsModel.SetData("IsToggleable", toggleable, index);
        }
    }

    function isToggleable(commandId){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            return root.commandsModel.GetData("IsToggleable", index);
        }

        return false;
    }

    function isToggled(commandId){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            return root.commandsModel.GetData("IsToggled", index);
        }

        return false;
    }

    function setToggled(commandId, toggled){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            root.commandsModel.SetData("IsToggled", toggled, index);
        }
    }

    function commandExists(commandId){
        let index = root.getCommandIndex(commandId);

        return index >= 0;
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
}
