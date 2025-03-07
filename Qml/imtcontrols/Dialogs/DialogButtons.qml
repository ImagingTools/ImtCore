import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import '../Buttons'

Item {
    id: buttonsDialogContainer;

    width: rowButtons.width;
    height: 30;

    property alias buttons: buttonsRepeater.model;
    property int count: buttonsRepeater.count;

	signal buttonClicked(int buttonId);

    function clearButtons(){
        buttonsRepeater.model = 0;
    }

    function addButton(buttonObj){
        buttonsRepeater.model.append(buttonObj)
    }

    function setButtonState(buttonId, state){
        for (let i = 0; i < buttonsRepeater.model.count; i++){
            let id = buttonsRepeater.model.get(i).Id;
            if (id == buttonId){
                buttonsRepeater.model.setProperty(i, "Enabled", state);
                break;
            }
        }
    }

    function getButtonState(buttonId){
        for (let i = 0; i < buttonsRepeater.model.count; i++){
            let id = buttonsRepeater.model.get(i).Id;
            if (id == buttonId){
                return buttonsRepeater.model.get(i).Enabled;
            }
        }

        return false;
    }

    Row {
        id: rowButtons;
        height: buttonsDialogContainer.height;
        spacing: Style.sizeMainMargin;

        Repeater {
            id: buttonsRepeater;
            model: ListModel {
            }

            delegate:
                Button{
                id: buttonContainer;
                enabled: model.Enabled;
                focus: model.Active !==undefined ? model.Active: false;
                text: model.Name;
				widthFromDecorator: true;
                onClicked: {
                    buttonsDialogContainer.buttonClicked(model.Id);
                }
            }//delegate
        }
    }
}

