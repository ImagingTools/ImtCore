import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
	id: root;
	bottomComp: !acceptableInput && showErrorWhenInvalid ? errorComp : undefined;
	
	property string text;
	property bool textInputFocus: false;
	property var textInputMask;
	property var textInputValidator;
	property bool readOnly: false;
	property bool acceptableInput: true;
	property int maximumLength: -1;
	property string placeHolderText;
	property string borderColor;
	property int echoMode: TextInput.Normal;
	
	property string errorText: qsTr("Enter the correct value");
	property bool showErrorWhenInvalid: false;

	property int textSize: Style.fontSizeM;
	property int placeHolderTextSize: textSize;
	
	signal accepted();
	signal cancelled();
	signal textEdited();
	signal editingFinished();
	
	onTextInputValidatorChanged: {
		if (textInputValidator && controlItem){
			controlItem.textInputValidator = textInputValidator;
		}
	}
	
	onMaximumLengthChanged: {
		if (controlItem){
			controlItem.maximumLength = maximumLength;
		}
	}
	
	onTextChanged: {
		if (controlItem){
			controlItem.text = text;
		}
	}
	
	onEchoModeChanged: {
		if (controlItem){
			controlItem.echoMode = echoMode;
		}
	}
	
	onBorderColorChanged: {
		if (controlItem){
			controlItem.borderColor = borderColor;
		}
	}
	
	onControlItemChanged: {
		if (controlItem){
			if (textInputValidator){
				controlItem.textInputValidator = textInputValidator;
			}
			
			if (maximumLength >= 0){
				controlItem.maximumLength = maximumLength;
			}
		}
	}
	
	onReadOnlyChanged: {
		if (controlItem){
			controlItem.readOnly = readOnly;
		}
	}
	
	controlComp: textInputComp;
	
	Component {
		id: textInputComp;
		CustomTextField {
			width: root.controlWidth;
			height: root.controlHeight;
			objectName: "TextField"
			
			text: root.text;
			
			// textInputFocus: root.textInputFocus;
			readOnly: root.readOnly
			placeHolderText: root.placeHolderText;
			echoMode: root.echoMode;
			textInputValidator: root.textInputValidator ? root.textInputValidator : null;
			textSize: root.textSize
			placeHolderTextSize: root.placeHolderTextSize
			
			onAccepted: {
				root.accepted();
			}
			
			onEditingFinished: {
				root.editingFinished();
			}
			
			onCancelled: {
				root.cancelled();
			}
			
			onTextEdited: {
				root.textEdited();
			}
			
			onTextChanged: {
				if(root.text !== text){
					root.text = text
				}
			}
			
			onAcceptableInputChanged: {
				root.acceptableInput = acceptableInput;
			}
		}
	}
	
	Component {
		id: errorComp;
		
		Text {
			text: root.errorText;
			color: Style.errorTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
		}
	}
}

