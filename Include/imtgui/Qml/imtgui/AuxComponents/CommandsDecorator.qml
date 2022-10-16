import QtQuick 2.12
import Acf 1.0

Item {
    id: commandsDecoratorContainer;

    height: 30;
    width: rowCommands.width;

    property string commandsId;

    function setCommandsModel(parameters){
        console.log("CommandsDecorator setCommandsModel", parameters, commandsDecoratorContainer);
        let model = parameters["Model"];
        let commId = parameters["CommandsId"];

        commandsDecoratorContainer.commandsId = commId;
        repeaterCommands.model = model;
    }

    Row {
        id: rowCommands;

        height: parent.height;

        spacing: 5;

        Repeater {
            id: repeaterCommands;

            delegate: TopButton {
                id: topButtonDelegate;

                text: model.Name;
                isEmpty: model.Name === "";
                imageSource: "../../../../" + "Icons/" + Style.theme + "/" + model.Icon + "_" + "Off" + "_" + model.Mode + ".svg";
                fontName: Style.fontFamily;

                checkable: model.Mode == "Normal";

                visible: model.Visible;

                onClicked: {
                    console.log("model.Visible", model.Visible);
                    Events.sendEvent(commandsDecoratorContainer.commandsId + "CommandActivated", model.Id);
                }
            }
        }
    }
}
