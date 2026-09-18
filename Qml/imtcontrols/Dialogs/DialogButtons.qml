import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import '../Buttons'

Item {
    id: buttonsDialogContainer;

    width: rowButtons.width;
	height: Style.controlHeightM;

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
            let id = buttonsRepeater.model.get(i).id;
            if (id == buttonId){
                buttonsRepeater.model.setProperty(i, "enabled", state);
                break;
            }
        }
    }

    function getButtonState(buttonId){
        for (let i = 0; i < buttonsRepeater.model.count; i++){
            let id = buttonsRepeater.model.get(i).id;
            if (id == buttonId){
                return buttonsRepeater.model.get(i).enabled;
            }
        }

        return false;
    }

    Row {
        id: rowButtons;
        height: buttonsDialogContainer.height;
        spacing: Style.marginM;

        Repeater {
            id: buttonsRepeater;
            model: ListModel {
            }

            delegate:
                Button{
                id: buttonContainer;
                enabled: model.enabled;
                focus: model.active !==undefined ? model.active: false;
                text: model.name;
				widthFromDecorator: true;
				decorator: Component {
					ButtonDecorator {
						// Dialog buttons sit on a near-white chrome and use their own white-based
						// idle/hover language (unlike the panel-tuned Style.buttonColor family), so
						// disabled here uses alternateBaseColor rather than the shared
						// Style.buttonInactiveColor - which is tuned to recede against panel
						// backgrounds, not a white dialog, and would otherwise render darker than
						// the dialog chrome behind it.
						color: !baseElement ? "transparent" :
							!baseElement.enabled ? Style.alternateBaseColor :
							baseElement.down || baseElement.checked ? Style.selectedColor :
							baseElement.hovered ? Style.alternateBaseColor :
							Style.baseColor;
					}
				}
                onClicked: {
					let retId = model.id || model.Id || ""
                    buttonsDialogContainer.buttonClicked(retId);
                }
            }//delegate
        }
    }
}

