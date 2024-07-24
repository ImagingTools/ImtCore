import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

QtObject {
    id: root;

    property TreeItemModel commandsModel: TreeItemModel {}
    property Item commandsView: null;

    property string uuid;

    signal commandModeChanged(string commandId, bool newMode);

    property bool isReady: false;

    onCommandModeChanged: {
        Events.sendEvent("CommandsEnabledChanged", {"Model": root.commandsModel, "CommandId": uuid});
    }

    function commandIsEnabled(commandId){
        if(!root.commandsModel){
            return false;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            let isEnabled = root.commandsModel.getData("IsEnabled", i);

            if (currentCommandId == commandId){
                return isEnabled;
            }
        }

        return false;
    }

    function updateModel(){}

    function setCommandIsEnabled(commandId, isEnabled){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            if (currentCommandId == commandId){
                let currentIsEnabled = root.commandsModel.getData("IsEnabled", i);
                if (currentIsEnabled !== isEnabled){
                    root.commandsModel.setData("IsEnabled", isEnabled, i);
                }
            }
        }
    }

    function setCommandIcon(commandId, icon){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.setData("Icon", icon, i);
                break;
            }
        }
    }

    function setCommandName(commandId, commandName){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.setData("Name", commandName, i);
                break;
            }
        }
    }

    function setCommandNotification(commandId, notification){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.setData("Status", notification, i);
                break;
            }
        }
    }

    function setCommandVisible(commandId, isVisible){
        if(!root.commandsModel){
            return;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            if (currentCommandId === commandId){
                root.commandsModel.setData("Visible", isVisible, i);
                break;
            }
        }
    }

    function getCommandIndex(commandId){
        if(!root.commandsModel){
            return -1;
        }

        for (let i = 0; i < root.commandsModel.getItemsCount(); i++){
            let currentCommandId = root.commandsModel.getData("Id", i);
            if (currentCommandId === commandId){
                return i;
            }
        }

        return -1;
    }

    function setIsToggleable(commandId, toggleable){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            root.commandsModel.setData("IsToggleable", toggleable, index);
        }
    }

    function isToggleable(commandId){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            return root.commandsModel.getData("IsToggleable", index);
        }

        return false;
    }

    function isToggled(commandId){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            return root.commandsModel.getData("IsToggled", index);
        }

        return false;
    }

    function setToggled(commandId, toggled){
        let index = root.getCommandIndex(commandId);

        if (index >= 0){
            root.commandsModel.setData("IsToggled", toggled, index);
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

        for (let i = 0; i < model.getItemsCount(); i++){
            model.setData("IsToggleable", false, i);
            model.setData("IsToggled", false, i);
        }
    }
}
