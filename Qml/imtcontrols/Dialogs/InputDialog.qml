import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: inputDialogContainer;
	
	width: 300;
	
	property string message;
	property string inputValue;
	property int contentWidth: 300;
	
	property string placeHolderText;
	
	Component.onCompleted: {
		addButton(Enums.ok, qsTr("OK"), true)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}
	
	Keys.onPressed: {
		if (event.key === Qt.Key_Return){
			inputDialogContainer.finished(Enums.ok);
		}
	}
	
	onFinished: {
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
		} }
}

