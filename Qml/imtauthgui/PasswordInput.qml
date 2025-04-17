import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Column {
	id: root;
	spacing: Style.sizeLargeMargin;
	property alias oldPassword: currentPasswordInput.text;
	property alias newPassword: newPasswordInput1.text;
	property bool accepted: false;
	property bool currentPasswordInputVisible: true;
	
	onFocusChanged: {
		if (root.focus){
			currentPasswordInput.focus = root.focus;
		}
	}
	
	function checkPassword(){
		root.accepted = newPasswordInput1.text !== "" && newPasswordInput1.text == newPasswordInput2.text;
		
		newPasswordInput1.acceptableInput = root.accepted;
		newPasswordInput2.acceptableInput = root.accepted;
		
		newPasswordInput2.bottomComp = root.accepted ? emptyComp : errorComp;
	}
	
	Component.onCompleted: {
		if (currentPasswordInputVisible){
			currentPasswordInput.forceActiveFocus();
		}
		else{
			oldPasswordGroup.forceActiveFocus();
		}
	}
	
	Component {
		id: errorComp;
		
		Text {
			text: qsTr("Passwords don't match");
			color: Style.errorTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeNormal;
		}
	}
	
	Component {
		id: emptyComp;
		
		BaseText {
		}
	}
	
	TextInputElementView {
		id: currentPasswordInput;
		width: parent.width;
		name: qsTr("Current password");
		echoMode: TextInput.Password;
		placeHolderText: qsTr("Enter the current password");
		visible: root.currentPasswordInputVisible;
		onVisibleChanged: {
			if (!visible){
				text = " ";
			}
		}
		
		onEditingFinished: {
		}
		
		KeyNavigation.tab: newPasswordInput1;
		KeyNavigation.backtab: newPasswordInput2;
	}
	
	GroupElementView {
		id: oldPasswordGroup;
		width: parent.width;
		TextInputElementView {
			id: newPasswordInput1;
			name: qsTr("New password");
			echoMode: TextInput.Password;
			placeHolderText: qsTr("Enter the new password");
			readOnly: currentPasswordInput.visible && currentPasswordInput.text === "";
			onTextChanged: {
				root.checkPassword();
			}
			KeyNavigation.tab: newPasswordInput2;
			KeyNavigation.backtab: currentPasswordInput.visible ? currentPasswordInput : newPasswordInput2;
		}
		
		TextInputElementView {
			id: newPasswordInput2;
			name: qsTr("Confirm password");
			echoMode: TextInput.Password;
			placeHolderText: qsTr("Confirm password");
			readOnly: newPasswordInput1.readOnly;
			bottomComp: emptyComp;
			onTextChanged: {
				root.checkPassword();
			}
			KeyNavigation.tab: currentPasswordInput.visible ? currentPasswordInput : newPasswordInput1;
			KeyNavigation.backtab: newPasswordInput1;
		}
	}
}


