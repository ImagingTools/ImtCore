import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0

ControlBase {
	id: dialogContainer;

	width:  !decorator_ ? 0 :  decorator_.width ;
	height:  !decorator_ ? 0 :  decorator_.height ;

	decorator: Style.dialogDecorator;
	property var decoratorItem: null;

	property string title;
	property string bodySource;
	property string topPanelSource;
	property bool forceFocus: false;

	property bool centered: true;
	property bool hasIcon: true;
	property bool modal: false;

	property Item root: null;
	property Item rootItem: null;

	property ListModel buttonsModel: ListModel{};
	property int buttonsModelCount: buttonsModel.count;
	property int notClosingButtons;
	property int buttonIds: 0;//81920;

	property string backgroundColor: Style.dialogBackgroundColor;
	property int radius: 0;

	property bool canMove: true;
	property bool canResize: false;
	property bool decoratorVisible: !decorator_ ? true: decorator_.visible;

	property bool closeAfterPause: false;
	property int displayDuration: 2000;

	property Component topPanelComp: Style.topPanelDialogDecorator;
	property Component topPanelDefault: Component{TopPanelDialog{}};
	property Component contentComp: null;

	property Item topPanel: null;
	property Item contentItem: null;
	property Item bodyItem: null;
	property Item buttons: null;

	signal finished(int buttonId);
	signal started();
	signal localizationChanged(string language);

	signal accepted();
	signal canceled();
	signal applied();
	signal closed();

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", dialogContainer.onLocalizationChanged);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", dialogContainer.onLocalizationChanged);
	}

	function addButton(id, name, enabled){
		if (enabled == undefined){
			enabled = true
		}

		buttonsModel.append({id: id, name:name, enabled: enabled})
	}

	function insertButton(id, name, enabled, index){
		buttonsModel.insert(index, {id: id, name:name, enabled: enabled})
	}

	function removeButton(id){
		for(let i = 0; i < buttonsModel.count; i++){
			let buttonId = buttonsModel.get(i).id;
			if (buttonId === id){
				buttonsModel.remove(i);
				break;
			}
		}
	}

	function clearButtons(){
		buttonsModel.clear();
	}

	function setButtonName(id, name){
		for(let i = 0; i < buttonsModel.count; i++){
			let buttonId = buttonsModel.get(i).id;
			if (buttonId === id){
				buttonsModel.setProperty(i, "name", name);
				break;
			}
		}
	}
	
	function buttonIsEnabled(buttonId){
		for(let i = 0; i < buttonsModel.count; i++){
			let id = buttonsModel.get(i).id
			if (buttonId === id){
				return buttonsModel.get(i).enabled
			}
		}
		
		return false
	}

	function setButtonEnabled(id, enabled){
		for(let i = 0; i < buttonsModel.count; i++){
			let buttonId = buttonsModel.get(i).id;
			if (buttonId === id){
				buttonsModel.setProperty(i, "enabled", enabled);
				break;
			}
		}
	}

	function onLocalizationChanged(language){
		dialogContainer.localizationChanged(language);
	}

	onRootChanged: {
		if (root && root.backgroundItem){
			root.backgroundItem.opacity = 0.4;
			if(dialogContainer.closeAfterPause){
				closeAnim.start();
			}
		}
	}

	onFinished: {
		if (dialogContainer.root){
			if(!(dialogContainer.notClosingButtons & buttonId)){
				dialogContainer.root.closeDialog();
			}
		}
	}

	onAccepted: {
		finished(Enums.ok)
	}
	onCanceled: {
		finished(Enums.cancel)
	}

	onDecoratorChanged: {
		if(decorator_){
			if(decorator_.accepted !==undefined){
				decorator_.accepted.connect(this.accepted);
			}
			if(decorator_.rejected !==undefined){
				decorator_.rejected.connect(this.canceled);
			}
			if(decorator_.applied !==undefined){
				decorator_.applied.connect(this.applied);
			}

		}
	}

	onButtonsModelChanged: {
		setButtonIds();
	}

	onButtonsModelCountChanged: {
		setButtonIds();
	}

	function setButtonIds(){
		if(!buttonsModel){
			return;
		}
		let buttonIds = 0;
		for(let i = 0; i < dialogContainer.buttonsModel.count; i++){
			let id = dialogContainer.buttonsModel.get(i).id;
			if(i == 0){
				buttonIds = id;
			}
			else {
				buttonIds = buttonIds | id;
			}
		}

		dialogContainer.buttonIds = buttonIds;
	}

	Shortcut {
		sequence: "Escape";
		enabled: true;
		onActivated: {
			dialogContainer.finished(Enums.cancel)
		}
	}

	PauseAnimation {
		id: closeAnim;

		duration: dialogContainer.displayDuration;
		onFinished: {
			if(dialogContainer.root){
				dialogContainer.closed()
				dialogContainer.root.closeDialog();
			}
		}
	}

	ResizeItem{
		anchors.fill: dialogContainer;

		visible: dialogContainer.canResize;

		targetItem: dialogContainer;
		onSizeChanged: {
			if(dialogContainer.contentItem){
				if(deltaWidth !== 0){
					dialogContainer.contentItem.width += deltaWidth;
				}
				if(deltaHeight !== 0){
					dialogContainer.contentItem.height += deltaHeight;
				}
			}
		}
	}

}
