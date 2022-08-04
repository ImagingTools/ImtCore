import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    onCommandsIdChanged: {
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    function commandHandle(commandId){
        console.log("AccountCollectionCommands commandActivated", commandId);
        container.commandHandleBase(commandId);
    }
}

