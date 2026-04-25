import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ControlBase {
	id: dialogContainer;
	objectName: "Dialog"

	width:  !decorator_ ? 0 :  decorator_.width ;
	height:  !decorator_ ? 0 :  decorator_.height ;

	decorator: Style.dialogDecorator;
	property var decoratorItem: null;

	property string title;
	property string bodySource;
	property string topPanelSource;
	property bool forceFocus: false;

	property bool centered: true;
	property bool pinned: false;
	property bool hasIcon: true;
	property bool modal: false;

	// For closing in Main dialog manager
	property string uuid: ""
	// Main dialog manager reference
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

	// Close policy – mirrors QtQuick.Controls.Popup.
	// Combine flags with |.
	// NoAutoClose=0, CloseOnEscape=1, CloseOnPressOutside=2, CloseOnPressOutsideParent=4
	property int closePolicy: Dialog.CloseOnEscape

	enum ClosePolicyFlag {
		NoAutoClose               = 0,
		CloseOnEscape             = 1,
		CloseOnPressOutside       = 2,
		CloseOnPressOutsideParent = 4
	}

	property bool escapeEnabled: (closePolicy & Dialog.CloseOnEscape) !== 0;

	property bool closeAfterPause: false;
	property int displayDuration: 2000;

	// Standard buttons – a bitmask of Enums.ButtonType values.
	// When set, buttons are automatically added to buttonsModel.
	property int standardButtons: Enums.noButton

	onStandardButtonsChanged: _applyStandardButtons()

	property Component topPanelComp: Style.topPanelDialogDecorator;
	property Component topPanelDefault: Component{TopPanelDialog{}};
	property Component contentComp: null;
	property Component selfComp: null;

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
	
	LocalizationEvent {
		onLocalizationChanged: {
			dialogContainer.onLocalizationChanged(langId)
		}
	}

	function _applyStandardButtons(){
		buttonsModel.clear();
		if (standardButtons === Enums.noButton)
			return
		// Ordered list of standard button definitions [id, label]
		let defs = [
			[Enums.ok,              qsTr("OK")],
			[Enums.open,            qsTr("Open")],
			[Enums.save,            qsTr("Save")],
			[Enums.saveAll,         qsTr("Save All")],
			[Enums.yes,             qsTr("Yes")],
			[Enums.yesToAll,        qsTr("Yes to All")],
			[Enums.no,              qsTr("No")],
			[Enums.noToAll,         qsTr("No to All")],
			[Enums.apply,           qsTr("Apply")],
			[Enums.reset,           qsTr("Reset")],
			[Enums.restoreDefaults, qsTr("Restore Defaults")],
			[Enums.discard,         qsTr("Discard")],
			[Enums.abort,           qsTr("Abort")],
			[Enums.retry,           qsTr("Retry")],
			[Enums.ignore,          qsTr("Ignore")],
			[Enums.close,           qsTr("Close")],
			[Enums.cancel,          qsTr("Cancel")],
			[Enums.help,            qsTr("Help")]
		]
		for (let i = 0; i < defs.length; i++) {
			let id   = defs[i][0]
			let name = defs[i][1]
			if (standardButtons & id)
				addButton(id, name, true)
		}
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
		if (root){
			if(root.backgroundItem){
				root.backgroundItem.opacity = 0.4;
			}
			if(dialogContainer.closeAfterPause){
				if(!closeAnim.running){
					closeAnim.start();
				}
			}
		}
	}

	onFinished: {
		if (dialogContainer.root){
			if(!(dialogContainer.notClosingButtons & buttonId)){
				if(dialogContainer.selfComp){
					dialogContainer.root.closeByComp(dialogContainer.selfComp);
				}
				else {
					dialogContainer.root.closeDialog();
				}
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
		enabled: dialogContainer.escapeEnabled;
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
				if(dialogContainer.selfComp){
					dialogContainer.root.closeByComp(dialogContainer.selfComp);
				}
				else {
					dialogContainer.root.closeDialog();
				}

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
