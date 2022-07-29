import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: topCenterPanelDecorator;

    CommandsDecorator {
        id: commandsDecorator;

        anchors.horizontalCenter: parent.horizontalCenter;

        Component.onCompleted: {
            Events.subscribeEvent("CommandsModelChanged", commandsDecorator.setCommandsModel);
        }
    }
}

