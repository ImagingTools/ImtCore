import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

Rectangle {
	id: window;
	anchors.fill: parent;
	color: Style.backgroundColor2;
	property string localSettings;
	
	signal saveSettings(string json);
	signal settingsSaved();
	signal settingsSaveFailed();
	
	property ParamsSet paramsSet: ParamsSet {}
	
	onLocalSettingsChanged: {
		console.log("onLocalSettingsChanged", localSettings)
		
		paramsSet.createFromJson(localSettings)

		preferenceDialog.paramsSet = paramsSet;
	}
	
	onSaveSettings: {
		loading.start()
	}
	
	onSettingsSaved: {
		buttons.setButtonState(Enums.apply, false);
		loading.stop()
	}
	
	onSettingsSaveFailed: {
		loading.stop()
	}
	
	Component.onCompleted: {
		Events.subscribeEvent("StartLoading", loading.start);
		Events.subscribeEvent("StopLoading", loading.stop);
		
		Style.setDecorators(decorators)
	}
	
	property Decorators decorators: decorators_
	
	Decorators {
		id: decorators_
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("StartLoading", loading.start);
		Events.unSubscribeEvent("StopLoading", loading.stop);
	}
	
	MouseArea{
		anchors.fill: parent;
		onClicked: {
			preferenceDialog.visible = true;
		}
	}
	
	Preference {
		id: preferenceDialog;
		
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: buttons.top;
		
		onEditorModelDataChanged: {
			buttons.setButtonState(Enums.apply, true);
		}
	}
	
	DialogButtons {
		id: buttons;
		
		anchors.bottom: parent.bottom;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginM;
		anchors.bottomMargin: Style.marginM;
		
		Component.onCompleted: {
			buttons.addButton({"id":Enums.apply, "name": qsTr("Apply"), "enabled": false});
			buttons.addButton({"id":Enums.close, "name": qsTr("Close"), "enabled": true});
		}
		
		onButtonClicked: {
			if (buttonId == Enums.apply){
				window.saveSettings(window.paramsSet.toJson());
			}
			else if (buttonId == Enums.close){
				if (preferenceDialog.modelIsDirty){
					ModalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
				}
				else{
					Qt.quit();
				}
			}
		}
	}
	
	Component {
		id: saveDialog;
		
		MessageDialog {
			Component.onCompleted: {
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}
			
			onFinished: {
				if (buttonId == Enums.yes){
					window.saveSettings(window.paramsSet.toJson());
					Qt.quit();
				}
				else if (buttonId == Enums.no){
					Qt.quit();
				}
			}
		}
	}
	
	DialogManagerView {
		anchors.fill: parent;
		z: 30;
	}
	
	Loading {
		id: loading;
		anchors.fill: parent;
		visible: false;
	}
}


