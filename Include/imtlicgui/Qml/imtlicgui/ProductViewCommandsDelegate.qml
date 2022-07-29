import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    Component.onDestruction: {
        Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
    }

    onCommandsIdChanged: {
        console.log("ProductCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onSelectedIndexChanged: {
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        container.commandsProvider.changeCommandMode("Duplicate", mode);
    }

    function commandHandle(commandId){
        console.log("PackageCommands commandActivated", commandId);
        super.commandHandle(commandId);
    }
}

