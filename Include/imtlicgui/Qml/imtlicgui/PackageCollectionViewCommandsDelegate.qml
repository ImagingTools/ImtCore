import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    onModelChanged: {
        treeViewModel.updateModel();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
    }

    onCommandsIdChanged: {
        console.log("PackageCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    function commandHandle(commandId){
        console.log("PackageCommands commandActivated", commandId);
        container.commandHandleBase(commandId);
    }
}
