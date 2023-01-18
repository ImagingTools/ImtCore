import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: topCenterPanelDecorator;

    Component.onDestruction: {
        console.log("topCenterPanelDecorator onDestruction");
        Events.unSubscribeEvent("CommandsModelChanged", commandsDecorator.setCommandsModel);
    }

    CommandsDecorator {
        id: commandsDecorator;

        anchors.horizontalCenter: parent.horizontalCenter;

        onVisibleChanged: {
            if (!commandsDecorator.visible){
                Events.unSubscribeEvent("CommandsModelChanged", commandsDecorator.setCommandsModel);
            }
        }

        Component.onCompleted: {
            console.log("topCenterPanelDecorator onCompleted");
            Events.subscribeEvent("CommandsModelChanged", commandsDecorator.setCommandsModel);
        }
    }
}

