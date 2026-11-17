import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: inputDialogContainer;
	
	width: Style.sizeHintXS;
	
	property string message;
	property string inputValue;
	property int contentWidth: Style.sizeHintXS;
	
	property string placeHolderText;

	property bool selectTextOnStart: false;

	// Return in the text field finishes the dialog through InputDialogBody (onAccepted) and the
	// key then reaches Keys.onPressed below as well; the dialog must be finished only once.
	property bool isFinished: false;
	
	Component.onCompleted: {
		addButton(Enums.ok, qsTr("OK"), true)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}
	
	Keys.onPressed: {
		if (event.key === Qt.Key_Return){
			event.accepted = true;
			if (!inputDialogContainer.isFinished){
				inputDialogContainer.finished(Enums.ok);
			}
		}
	}
	
	onFinished: {
		inputDialogContainer.isFinished = true;
		if (buttonId == Enums.ok){
			inputDialogContainer.inputValue = inputDialogContainer.bodyItem.inputValue;
		}
	}
	
	onLocalizationChanged: {
		setButtonName(Enums.ok, qsTr("OK"))
		setButtonName(Enums.cancel, qsTr("Cancel"))
	}
	
	contentComp: Component { InputDialogBody {
			width: inputDialogContainer.width;
			message: inputDialogContainer.message;
			inputValue: inputDialogContainer.inputValue;
			placeHolderText: inputDialogContainer.placeHolderText;
			selectTextOnStart: inputDialogContainer.selectTextOnStart;
		} }
}

