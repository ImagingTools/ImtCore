import QtQuick 2.12
import Acf 1.0

Item {
    id: commandsDecoratorContainer;

    height: 30;
    width: rowCommands.width;

    property string commandsId;

    Component.onCompleted: {
        Events.subscribeEvent("CommandsDecoratorClear", clearModel);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("CommandsDecoratorClear", clearModel);
    }

    function setCommandsModel(parameters){
        console.log("CommandsDecorator setCommandsModel", parameters, commandsDecoratorContainer);
        let model = parameters["Model"];
        let commId = parameters["CommandsId"];

        commandsDecoratorContainer.commandsId = commId;
        repeaterCommands.model = model;
    }

    function clearModel(){
        repeaterCommands.model = 0;
    }

    Row {
        id: rowCommands;

        height: parent.height;

        spacing: 5;

        Repeater {
            id: repeaterCommands;

            delegate:
                BaseButton {

                id: topButtonDelegate;

                decoratorSource: "TopButtonDecorator.qml";
                //decoratorSource: "../../../../NeoPro/TopButtonDecoratorNeoPro.qml";

                //imageSource: "../../../../" + "Icons/" + Style.theme + "/" + model.Icon + "_" + "Off" + "_" + model.Mode + ".svg";
                imageSource: "../../../../" + "Icons/" + Style.theme + "/" + model.Icon + "_" + "Off" + "_" + model.Mode + ".svg";

                enabled:  model.Mode == "Normal";
                visible: model.Visible;

                text: model.Name;

                onClicked: {
                    console.log("model.Visible", model.Visible);
                    Events.sendEvent(commandsDecoratorContainer.commandsId + "CommandActivated", model.Id);
                }
            }
        }
    }
}
