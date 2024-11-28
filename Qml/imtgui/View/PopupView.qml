import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: container;

    // Main dialog manager reference
    property Item root: null;
    property TreeItemModel model;

    property bool hiddenBackground: true;
    property bool noMouseArea: false;
    property bool forceFocus: true;

    Component.onCompleted: {
        Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
    }

    function onBackgroundClicked(){
        if (root){
            root.closeDialog();
        }
    }

    function onAppSizeChanged(parameters){
        onBackgroundClicked()
    }
}


