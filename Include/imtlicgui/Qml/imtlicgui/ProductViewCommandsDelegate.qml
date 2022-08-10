import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdated", objectView.updateGui);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdated", objectView.updateGui);
    }

    onCommandActivated: {
        console.log("ProductsCommands onCommandActivated", commandId);
        if (commandId === "Duplicate"){
            let elements = container.tableData.elements;

            let selectedId = elements.GetData("Id", container.selectedIndex);
            let selectedName = elements.GetData("Name", container.selectedIndex);
            let selectedDescription = elements.GetData("Description", container.selectedIndex);

            let index = elements.InsertNewItem();
            elements.SetData("Name", "Copy of " + selectedName, index);
            elements.SetData("Id", selectedId, index);
            elements.SetData("Description", selectedDescription, index);
        }
    }

    onSelectedIndexChanged: {
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Duplicate", mode);
    }

    onSaved: {
        Events.sendEvent("TreeViewModelUpdate");
        Events.sendEvent(commandsId + "CollectionUpdateGui");
    }

    onClosed: {
        Events.sendEvent("LicenseFeaturesUpdate");
    }

//    RegExpValidator{
//        id: validatorId;
//        regExp: "[0-9][0-9]\.[0-9999]";
//    }
}

