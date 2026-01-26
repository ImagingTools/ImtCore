import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

Dialog {
	id: messageDialog
	
	title: qsTr("Preferences");

	width: Style.sizeHintXXXL;
	
	// notClosingButtons: Enums.apply;
	
	property ParamsSet paramsSet: ParamsSet {}
	// Required: SettingsController instance provided by parent component
	property SettingsController settingsController
	
	property bool hasChanges: false
	property var changedParams: ({})
	
	Component.onCompleted: {
		updateButtons()
	}
	
	onLocalizationChanged: {
		updateButtons()
	}
	
	property int rootWidth: ModalDialogManager.activeView.width
	
	onRootWidthChanged: {
		let newWidth = Math.min(rootWidth - 2 * Style.marginM, 1000);
		
		if (messageDialog.width !== newWidth){
			messageDialog.width = newWidth;
		}
	}
	
	function updateButtons(){
		clearButtons()
		addButton(Enums.apply, qsTr("Apply"), false)
		addButton(Enums.cancel, qsTr("Close"), true)
	}
	
	onFinished: {
		if (buttonId == Enums.apply){
			setButtonEnabled(Enums.apply, false)
			setButtonName(Enums.cancel, qsTr("Close"))

			// Save all changed params efficiently
			for (let paramId in changedParams){
				if (changedParams[paramId]){
					settingsController.saveParam(paramId)
				}
			}
			
			changedParams = {}
			hasChanges = false
		}
	}
	
	contentComp: Component {
		Preference {
			width: messageDialog.width;
			height: Style.sizeHintL
			paramsSet: messageDialog.paramsSet
			settingsController: messageDialog.settingsController
			
			onEditorModelDataChanged: {
				if (!messageDialog.hasChanges){
					messageDialog.buttons.setButtonState(Enums.apply, true);
					messageDialog.buttonsModel.setProperty(1, "name", qsTr("Cancel"));
					messageDialog.hasChanges = true
				}
				
				// Extract main param ID: nested params use "parent/child" format, 
				// we only track the parent ID to avoid redundant saves
				let mainParamId = paramId.includes("/") ? paramId.split("/")[0] : paramId
				messageDialog.changedParams[mainParamId] = true
			}
		}
	}
}
