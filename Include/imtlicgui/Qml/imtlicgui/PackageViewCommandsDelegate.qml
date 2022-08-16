import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentWorkspaceCommandsDelegate {
    id: container;

    Component.onCompleted: {
        Events.subscribeEvent("TreeViewModelUpdated", updateGui)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("TreeViewModelUpdated", updateGui)
    }

    onEntered: {
        objectModel.SetData("Id", value);
        objectModel.SetData("Name", value);
    }

    onSaved: {
        if (gqlModelQueryType == "Add"){
            Events.sendEvent("TreeViewModelUpdate");
        }

        Events.sendEvent("FeatureDependenciesUpdate");
    }

    onClosed: {
//        Events.sendEvent("TreeViewModelUpdate");
    }

    onRemoved: {
        treeViewModel.removeFeatureInTreeViewModel(objectView.itemId, itemId);
    }

    onEdited: {
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

