import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0

CommandsRepresentationProvider {
    id: container;

    property CommandsPanel commandsView: null;

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

    function setCommandIsEnabled(commandId, isEnabled){
        for (let i = 0; i < commandsModel.getItemsCount(); i++){
            let found = false;
            let subElements = commandsModel.getData("SubElements", i)
            if (subElements){
                for (let j = 0; j < subElements.getItemsCount(); j++){
                    let id = subElements.getData("Id", j)
                    if (id === commandId){
                        subElements.setData("IsEnabled", isEnabled, j)
                        found = true;
                        break;
                    }
                }
            }

            if (found){
                break;
            }
        }

        if (container.commandsView){
            container.commandsView.setCommandData(commandId, "IsEnabled", isEnabled);
        }
    }
}

