import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

CommandsController {
    id: commandsProviderContainer;

    property string commandId;

    onCommandIdChanged: {
        updateModel();
    }

    function updateModel(){
        if (commandId === ""){
            console.error("Unable to update commands model. Command-ID is empty.");
            return;
        }

		modelCommands.gqlCommandId = commandId + "Commands"
		commandsProviderContainer.modelCommands.send();
    }

    function getHeaders(){
        return {};
    }

	property GqlRequestSender modelCommands: GqlRequestSender {
		function createQueryParams(query, params){
			commandsProviderContainer.isReady = false;
		}

		function getHeaders(){
			return commandsProviderContainer.getHeaders();
		}

		function onResult(data){
			commandsProviderContainer.setAdditionalProperties(data);
			commandsProviderContainer.commandsModel = data;

			commandsProviderContainer.isReady = true;
		}
    }
}

