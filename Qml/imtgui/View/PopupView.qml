import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: container;

    // Main dialog manager reference
    property Item root: null;
    property TreeItemModel model;

    onRootChanged: {
        if (root){
            root.backgroundItem.opacity = 0;
            Events.subscribeEvent("DialogBackgroundClicked", root.closeDialog)
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
        Events.unSubscribeEvent("DialogBackgroundClicked", root.closeDialog)
    }

    function onAppSizeChanged(parameters){
        if (root){
            root.closeDialog();
        }
    }
}


