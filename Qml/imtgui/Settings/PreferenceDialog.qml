import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

Dialog {
	id: messageDialog
	
	title: qsTr("Preferences");

	width: 1000;
	
	// notClosingButtons: Enums.apply;
	
	property ParamsSet paramsSet: ParamsSet {}
	property SettingsController settingsController: SettingsController {}
	
	Component.onCompleted: {
		updateButtons()
	}
	
	onLocalizationChanged: {
		updateButtons()
	}
	
	property int rootWidth: ModalDialogManager.activeView.width
	
	onRootWidthChanged: {
		let newWidth = Math.min(rootWidth - 2 * Style.sizeMainMargin, 1000);
		
		if (messageDialog.width !== newWidth) {
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

			for (let key in paramIdsChanges){
				let changes = paramIdsChanges[key]
				
				settingsController.saveParam(key)
			}
			
			paramIdsChanges = {}
		}
	}
	
	property var paramIdsChanges: ({})
	
	contentComp: Component {
		Preference {
			width: messageDialog.width;
			height: 500
			paramsSet: messageDialog.paramsSet
			settingsController: messageDialog.settingsController
			
			onEditorModelDataChanged: {
				messageDialog.buttons.setButtonState(Enums.apply, true);
				messageDialog.buttonsModel.setProperty(1, "name", qsTr("Cancel"));
				
				let changeObj = {}
				let mainParamId = paramId
				if (paramId.includes("/")){
					let ids = paramId.split("/")
					mainParamId = ids[0]
				}
				
				if (mainParamId in messageDialog.paramIdsChanges){
					if (!messageDialog.paramIdsChanges[mainParamId].includes(paramId)){
						messageDialog.paramIdsChanges[mainParamId].push(paramId)
					}
				}
				else{
					messageDialog.paramIdsChanges[mainParamId] = [paramId]
				}
			}
		}
	}
}
