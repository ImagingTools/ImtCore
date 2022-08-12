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

    onEntered: {
        objectModel.SetData("Id", value);
        objectModel.SetData("Name", value);
    }

    onSaved: {
        console.log("PackageView onSaved");
        Events.sendEvent("TreeViewModelUpdate");
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
}

