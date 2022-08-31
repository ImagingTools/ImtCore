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
        Events.sendEvent("TreeViewModelUpdate");
        Events.sendEvent("FeatureDependenciesUpdate");
    }
}

