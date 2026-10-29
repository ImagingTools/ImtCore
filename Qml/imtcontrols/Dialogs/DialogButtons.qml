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

	// The affirmative action is the accent button, as in VS Code dialogs; the rest stay secondary.
	function variantForButton(buttonId, variant){
		if (variant !== undefined && variant !== ""){
			return variant;
		}

		if (buttonId === Enums.ok || buttonId === Enums.yes || buttonId === Enums.save || buttonId === Enums.apply){
			return "primary";
		}

		return "default";
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
				variant: buttonsDialogContainer.variantForButton(model.id, model.variant);
				widthFromDecorator: true;
				decorator: Component {
					ButtonDecorator {
						widthDefault: Style.buttonWidthXL;
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

