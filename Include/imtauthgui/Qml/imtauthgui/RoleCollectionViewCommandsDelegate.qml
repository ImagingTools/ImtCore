import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    onCommandsProviderChanged: {
        commandsProvider.modelLoaded.connect(container.updateCommands);
    }

    Component.onDestruction: {
        commandsProvider.modelLoaded.disconnect(container.updateCommands);
    }

    function updateCommands(){
        console.log("RoleColelction updateCommands");

        let index = commandsProvider.commandsModel.InsertNewItem();

        commandsProvider.commandsModel.SetData("Id", "New", index);
        commandsProvider.commandsModel.SetData("IsEnabled", true, index);
    }
}
