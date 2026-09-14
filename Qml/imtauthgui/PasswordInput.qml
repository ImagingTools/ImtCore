import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Column {
	id: root;
	spacing: Style.marginXL;
	property alias oldPassword: currentPasswordInput.text;
	property alias newPassword: newPasswordInput1.text;
	property bool accepted: false;
	property bool currentPasswordInputVisible: true;
	property string login: "";
	property string errorText: "";

	// PasswordPolicyController instance, injected by the owning view. Without it
	// only the "passwords match" check applies.
	property var policy: null;

	onFocusChanged: {
		if (root.focus){
			currentPasswordInput.focus = root.focus;
		}
	}

	function checkPassword(){
		let matching = newPasswordInput1.text !== "" && newPasswordInput1.text == newPasswordInput2.text;
		let violatedRules = root.policy ? root.policy.validate(root.login, newPasswordInput1.text) : [];

		root.accepted = matching && (violatedRules.length === 0);

		if (!matching){
			root.errorText = qsTr("Passwords don't match");
		}
		else if (violatedRules.length > 0){
			root.errorText = root.policy.describeFailure(violatedRules, "");
		}
		else{
			root.errorText = "";
		}

		newPasswordInput1.acceptableInput = root.accepted;
		newPasswordInput2.acceptableInput = root.accepted;

		newPasswordInput2.bottomComp = (root.errorText === "") ? emptyComp : errorComp;
	}

	Component.onCompleted: {
		if (root.policy){
			root.policy.load();
		}

		if (currentPasswordInputVisible){
			currentPasswordInput.forceActiveFocus();
		}
		else{
			oldPasswordGroup.forceActiveFocus();
		}
	}

	// The policy usually arrives after the field was built, so re-check what is typed.
	Connections {
		target: root.policy ? root.policy : null;

		function onPolicyReceived() {
			root.checkPassword();
		}
	}

	Component {
		id: errorComp;

		Text {
			text: root.errorText;
			wrapMode: Text.WordWrap;
			color: Style.errorTextColor;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
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

	Text {
		width: parent.width;
		wrapMode: Text.WordWrap;
		text: root.policy ? root.policy.requirementsText() : "";
		visible: (text !== "") && (root.errorText === "");
		color: Style.inactiveTextColor;
		font.family: Style.fontFamily;
		font.pixelSize: Style.fontSizeM;
	}
}


