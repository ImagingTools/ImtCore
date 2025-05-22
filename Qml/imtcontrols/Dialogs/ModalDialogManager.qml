pragma Singleton

import QtQuick 2.12
import Acf 1.0
// import imtcontrols 1.0
import com.imtcore.imtqml 1.0
import QtQuick.Window 2.2

Item {
	id: container;
	
	property var backgroundItem: null;
	property Item activeView: null; // Active view for this Dialog Manager
	property Item topItem: null;
	property ListModel modalDialogModels: ListModel {}
	property alias count: container.modalDialogModels.count;
	
	property var dialogCallback: ({})
	
	Component.onDestruction: {
		modalDialogModels.clear();
	}
	
	/*
		This signal is emitted when finished top level dialog.
		\c finished.
	*/
	signal finished(int result);
	
	function openDialog(comp, parameters, mode, callback){
		let dialogMode = mode !== undefined ? mode : Style.dialogMode !== undefined ? Style.dialogMode: "Internal";
		
		/*for Windows style dialogs*/
		if(dialogMode == "External"){
			container.openWindow(comp, parameters);
		}
		else {
			if (callback){
				dialogCallback[comp] = callback;
			}
			
			modalDialogModels.append({"Component": comp, "Parameters": parameters});
		}
	}
	
	function closeDialog(index){
		if (!index){
			index = -1;
			
			if (modalDialogModels.count > 0){
				index = modalDialogModels.count - 1;
			}
		}
		
		if (index >= 0 && index < modalDialogModels.count){
			let c = modalDialogModels.get(index).Component;
			delete dialogCallback[c];
			
			modalDialogModels.remove(index);
		}
	}
	
	function closeByComp(comp){
		for (let i = 0; i < modalDialogModels.count; i++){
			let c = modalDialogModels.get(i).Component;
			if (c && c === comp){
				modalDialogModels.remove(i);
				
				delete dialogCallback[c];
				
				return true;
			}
		}
		
		return false;
	}
	
	function dialogIsOpened(dialogComp){
		for (let i = 0; i < modalDialogModels.count; i++){
			let c = modalDialogModels.get(i).Component;
			if (c && c === dialogComp){
				return true;
			}
		}
		
		return false;
	}
	
	function showInfoDialog(message){
		openDialog(infoDialog, {"message": message});
	}
	
	function showWarningDialog(message){
		openDialog(warningDialog, {"message": message});
	}
	
	function showCriticalDialog(message){
		openDialog(warningDialog, {"message": message});
	}
	
	function showErrorDialog(message){
		openDialog(warningDialog, {"message": message});
	}
	
	/*!
		\qmlmethod void ModalDialogManager::showConfirmationDialog(string title, string message, var callback = undefined, bool choiceIsRequired = false)
		\brief Opens the confirmation dialog.
	
		This method displays a confirmation dialog with a title, message, and optional callback to handle the result. 
		It is used to prompt the user for confirmation (e.g., Yes/No, OK/Cancel).
	
		\param title The title text shown at the top of the dialog.
		\param message The main message displayed in the body of the dialog.
		\param callback (optional) A function that will be called with the result of the user’s choice.
				It should accept one argument:
				- 'true' if the user confirmed the action (e.g., clicked "Yes" or "OK")
				- 'false' if the user canceled the action (e.g., clicked "No" or "Cancel")
				- 'undefined' if the dialog was closed in a way that doesn't produce a clear result (only if 'choiceIsRequired' is false)
		\param choiceIsRequired (optional) If true, the dialog cannot be closed without the user making a choice.
				Useful for preventing accidental dismissal (default is false).
	*/
	function showConfirmationDialog(title, message, callback, choiceIsRequired){
		if (!choiceIsRequired){
			choiceIsRequired = false
		}

		openDialog(confirmationDialogComp, {"message": message, "title": title, "choiceIsRequired": choiceIsRequired}, "Internal", callback);
	}
	
	property Component infoDialog: Component{
		ErrorDialog {
			title: qsTr("Information");
			onFinished: {}
		}
	}
	
	property Component warningDialog: Component{
		ErrorDialog {
			title: qsTr("Warning");
			onFinished: {}
		}
	}
	
	property Component criticalDialog: Component{
		CriticalErrorDialog {
			title: qsTr("Critical");
			onFinished: {}
		}
	}
	
	property Component confirmationDialogComp: Component{
		MessageDialog {
			property bool choiceIsRequired: false
			onStarted: {
				if (!choiceIsRequired){
					addButton(Enums.cancel, qsTr("Cancel"), true)
				}
			}
		}
	}
	
	/*for Windows style dialogs*/
	function openWindow(comp, parameters){
		let obj = windowComp.createObject(container);
		obj.parameters = parameters;
		obj.sourceComponent = comp;
		for (let key in parameters) {
			if(key == "modality"){
				obj[key] = parameters[key];
			}
		}
		obj.show();
	}
	
	property Component windowComp: Component {
		Window {
			id: windowObj;
			
			title: " ";
			
			flags: Qt.Dialog;
			modality: Qt.NonModal;//Qt.WindowModal
			
			width: 100;
			height: 100;
			
			property var parameters;
			property alias sourceComponent: windowLoader.sourceComponent;
			property Item item: !windowLoader.item ? null : windowLoader.item;
			
			function closeFunc(buttonId){
				windowObj.close();
			}
			
			Component.onDestruction:  {
			}
			
			onClosing:{
				windowObj.destroy();
			}
			
			onWidthChanged: {
				if(windowLoader.item){
					windowLoader.item.width = windowObj.width;
				}
			}
			
			onHeightChanged: {
				if(windowLoader.item){
					windowLoader.item.height = windowObj.height;
				}
			}
			
			Loader{
				id: windowLoader;
				
				onLoaded: {
					item.finished.connect(windowObj.closeFunc);
					windowObj.width = item.width;
					windowObj.height = item.height;
					
					for (let key in windowObj.parameters) {
						if(item[key] !== undefined){
							item[key] = windowObj.parameters[key];
						}
					}
				}
			}
		}
	}
}
