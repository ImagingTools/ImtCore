import QtQuick 2.12
import imtlicgui 1.0

ObjectView {
    id: container;

    anchors.fill: parent;

    Component.onCompleted: {
        console.log("container onCompleted");
        container.commandsDelegatePath = "PackageViewCommandsDelegate.qml";
        container.rightPanelTitle = qsTr("Dependencies");
        container.treeViewControllerPath = "PackageViewTreeViewController.qml";
    }
}
