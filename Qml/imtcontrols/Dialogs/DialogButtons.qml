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
				variant: model.variant !== undefined ? model.variant : "default";
				widthFromDecorator: true;
				decorator: Component {
					ButtonDecorator {
						id: dialogButtonDecorator

						// Dialog buttons sit on a near-white chrome, so the default variant keeps its
						// own white-based idle/hover language instead of the panel-tuned
						// Style.buttonColor family. Filled variants use the shared palette unchanged.
						variantColor: dialogButtonDecorator.isPrimary ? Style.buttonPrimaryColor : dialogButtonDecorator.isDanger ? Style.buttonDangerColor : Style.baseColor;
						variantHoverColor: dialogButtonDecorator.isPrimary ? Style.buttonPrimaryHoverColor : dialogButtonDecorator.isDanger ? Style.buttonDangerHoverColor : Style.alternateBaseColor;
						variantPressedColor: dialogButtonDecorator.isPrimary ? Style.buttonPrimaryPressedColor : dialogButtonDecorator.isDanger ? Style.buttonDangerPressedColor : Style.selectedColor;
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

