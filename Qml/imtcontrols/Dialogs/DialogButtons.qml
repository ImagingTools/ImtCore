import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import '../Buttons'

Item {
    id: buttonsDialogContainer;

    height: 30;
    width: rowButtons.width;

    property alias buttons: buttonsRepeater.model;
    property int count: buttonsRepeater.count;

    signal buttonClicked(string buttonId);

    function clearButtons(){
        buttonsRepeater.model = 0;
    }

    function addButton(buttonObj){
        console.log("buttonsDialogContainer addButton");
        buttonsRepeater.model.append(buttonObj)
    }

    function setButtonState(buttonId, state){
        console.log("setButtonState", buttonId, state);
        for (let i = 0; i < buttonsRepeater.model.count; i++){
            let id = buttonsRepeater.model.get(i).Id;
            console.log("id", id);

            if (id == buttonId){
                console.log("==");
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

//    ListModel {
//        id: buttonsRepeater.model;
//    }

    Row {
        id: rowButtons;

        height: buttonsDialogContainer.height;

        spacing: 10;

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

                onClicked: {
                    console.log("Button onClicked", model.Id);
                    buttonsDialogContainer.buttonClicked(model.Id);
                }
            }//delegate
        }
    }
}

