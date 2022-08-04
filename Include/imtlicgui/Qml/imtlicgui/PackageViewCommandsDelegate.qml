import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    onCommandsIdChanged: {
        console.log("PackageCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onVisibleChanged: {
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdated", objectView.updateGui);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdated", objectView.updateGui);
    }

    onSaved: {
        Events.sendEvent("TreeViewModelUpdate");
        Events.sendEvent(commandsId + "CollectionUpdateGui");
    }

    onClosed: {
        Events.sendEvent("TreeViewModelUpdate");
        Events.sendEvent("FeatureDependenciesUpdate");
    }

    onRemoved: {
        treeViewModel.removeFeatureInTreeViewModel(objectView.itemId, itemId);
    }

    onEdited: {
        console.log("onEdited");
        let oldId = itemId;
        let elementsModel = tableData.elements;

        let newId = elementsModel.GetData("Id", selectedIndex);
        let newName = elementsModel.GetData("Name", selectedIndex);

        if (oldId == ""){
            treeViewModel.addFeatureInTreeViewModel(objectView.itemId, newId, newName);
        }
        else{
            treeViewModel.updateTreeViewAfterFeatureEditing(objectView.itemId, oldId, newId, newName);
        }
    }

    function commandHandle(commandId){
        console.log("PackageCommands commandActivated", commandId);

        container.commandHandleBase(commandId);
    }
}

