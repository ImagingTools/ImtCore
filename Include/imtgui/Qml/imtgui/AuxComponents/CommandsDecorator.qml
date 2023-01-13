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

    Component{
        id: defaultButtonDecorator;
        TopButtonDecorator{}
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

                decorator: Style.topButtonDecorator !==undefined ? Style.topButtonDecorator: defaultButtonDecorator;
                imageSource: model.IsEnabled ? "../../../../Icons/" + Style.theme + "/" + model.Icon + "_Off_Normal.svg" :
                                       "../../../../Icons/" + Style.theme + "/" + model.Icon + "_Off_Disabled.svg";

                enabled: model.IsEnabled;

                visible: model.Visible;

                text: model.Name;

                onClicked: {
                    Events.sendEvent(commandsDecoratorContainer.commandsId + "CommandActivated", model.Id);
                }
            }
        }
    }
}
