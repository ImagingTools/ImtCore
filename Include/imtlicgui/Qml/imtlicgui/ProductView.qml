import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0

ObjectView {
    id: container;

    anchors.fill: parent;

    Component.onCompleted: {
        console.log("ProductView onCompleted");
        container.commandsDelegatePath = "ProductViewCommandsDelegate.qml";
        container.rightPanelTitle = qsTr("Features");
        container.treeViewControllerPath = "ProductViewTreeViewController.qml";
    }
}
