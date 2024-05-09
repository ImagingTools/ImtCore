import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

DecoratorBase {
    id: topCenterPanelDecorator;

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateCommandsGui", commandsDecorator.setCommandsModel);
    }

    CommandsDecorator {
        id: commandsDecorator;

        onVisibleChanged: {
            if (!commandsDecorator.visible){
                Events.unSubscribeEvent("UpdateCommandsGui", commandsDecorator.setCommandsModel);
            }
        }

        Component.onCompleted: {
            Events.subscribeEvent("UpdateCommandsGui", commandsDecorator.setCommandsModel);
        }
    }
}

