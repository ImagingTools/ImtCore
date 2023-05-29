import QtQuick 2.12
import Acf 1.0
import imtgui 1.0


Item {
    id: buttonsDialogContainer;

    height: 30;
    width: rowButtons.width;

    signal buttonClicked(string buttonId);

    function addButton(buttonObj){
        console.log("buttonsDialogContainer addButton");
        buttonsModel.append(buttonObj)
    }

    function setButtonState(buttonId, state){
        for (let i = 0; i < buttonsModel.count; i++){
            let id = buttonsModel.get(i).Id;
            if (id === buttonId){
                buttonsModel.setProperty(i, "Enabled", state);
                break;
            }
        }
    }

    function getButtonState(buttonId){
        for (let i = 0; i < buttonsModel.count; i++){
            let id = buttonsModel.get(i).Id;
            if (id === buttonId){
                return buttonsModel.get(i).Enabled;
            }
        }

        return false;
    }

    ListModel {
        id: buttonsModel;
    }

    Component{
        id: defaultButtonDecorator;
        CommonButtonDecorator{
            width: 70;
            height: 25;
        }
    }

    Row {
        id: rowButtons;

        height: buttonsDialogContainer.height;

        spacing: 10;

        Repeater {
            id: buttonsRepeater;

            model: buttonsModel;

            delegate:

                BaseButton{
                id: buttonContainer;

                enabled: model.Enabled;
                isFocused: model.Active !==undefined ? model.Active: false;

                text: model.Name;

                decorator: Style.commonButtonDecorator !==undefined ? Style.commonButtonDecorator : defaultButtonDecorator;

                onLoaded: {
                    buttonsDialogContainer.height = buttonContainer.height;
                }

                onClicked: {
                    console.log("Button onClicked", model.Id);
                    buttonsDialogContainer.buttonClicked(model.Id);
                }
            }//delegate
        }
    }
}
