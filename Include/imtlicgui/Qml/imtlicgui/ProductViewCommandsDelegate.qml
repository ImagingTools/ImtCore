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

    onVisibleChanged: {
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
        }
    }


    onCommandsIdChanged: {
        console.log("ProductCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onSelectedIndexChanged: {
        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        container.commandsProvider.changeCommandMode("Duplicate", mode);
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

    function commandHandle(commandId){
        console.log("ProductsCommands commandActivated", commandId);
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
        else if (commandId === "Save"){
            if (container.objectView.itemId  === ""){
                container.objectView.itemId = productModel.GetData("Id");
                container.gqlModelQueryType = "Add";
                container.gqlModelQueryTypeNotify = "addedNotification";
            }
            else{
                container.gqlModelQueryType = "Update";
                container.gqlModelQueryTypeNotify = "updatedNotification";
            }

            saveModel();
        }
        else{
            container.commandHandleBase(commandId);
        }
    }
}

