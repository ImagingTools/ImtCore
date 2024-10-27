import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0

CommandsRepresentationProvider {
    id: container;

    function commandExists(commandId){
        for (let i = 0; i < commandsModel.getItemsCount(); i++){
            let subElements = commandsModel.getData("SubElements", i)
            if (subElements){
                for (let j = 0; j < subElements.getItemsCount(); j++){
                    let id = subElements.getData("Id", j)
                    if (id === commandId){
                        return true;
                    }
                }
            }
        }

        return false;
    }

    function commandIsEnabled(commandId){
        if (container.commandsView){
            return container.commandsView.getCommandData(commandId, "IsEnabled");
        }

        return false;
    }

    function isToggled(commandId){
        if (container.commandsView){
            return container.commandsView.getCommandData(commandId, "IsToggled");
        }

        return false;
    }

    function setToggled(commandId, toggled){
        setCommandProperty(commandId, "IsToggled", toggled);
        if (container.commandsView){
            container.commandsView.setCommandData(commandId, "IsToggled", toggled);
        }
    }

    function setCommandIcon(commandId, icon){
        setCommandProperty(commandId, "Icon", icon);
        if (container.commandsView){
            container.commandsView.setCommandData(commandId, "Icon", icon);
        }
    }

    function setCommandName(commandId, commandName){
        setCommandProperty(commandId, "Name", commandName);
        if (container.commandsView){
            container.commandsView.setCommandData(commandId, "Name", commandName);
        }
    }

    function setCommandIsEnabled(commandId, isEnabled){
        setCommandProperty(commandId, "IsEnabled", isEnabled);
        if (container.commandsView){
            container.commandsView.setCommandData(commandId, "IsEnabled", isEnabled);
        }
    }

    function setCommandProperty(commandId, key, value){
        for (let i = 0; i < commandsModel.getItemsCount(); i++){
            let subElements = commandsModel.getData("SubElements", i)
            if (subElements){
                for (let j = 0; j < subElements.getItemsCount(); j++){
                    let id = subElements.getData("Id", j)
                    if (id === commandId){
                        subElements.setData(key, value, j)
                        return true;
                    }
                }
            }
        }

        return false;
    }
}

