import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0
import imtdocgui 1.0

Column {
    id: container;
	spacing: Style.size_largeMargin;

	property UserData userData;
	property bool readOnly: false;

	signal emitUpdateModel();
	signal emitUpdateGui();

	property alias usernameInput: usernameInput_;
	property alias passwordInput: passwordInput_;
	property alias confirmPasswordInput: confirmPassword;
	property alias changePasswordButton: changePasswordButton_;

	function updateGui(){
		usernameInput_.text = container.userData.m_username;
		nameInput.text = container.userData.m_name;
		mailInput.text = container.userData.m_email;
		passwordInput_.text = container.userData.m_password;
	}

	function updateModel(){
		container.userData.m_username = usernameInput_.text;
		container.userData.m_name = nameInput.text;
		container.userData.m_email = mailInput.text;
		container.userData.m_password = passwordInput_.text;
	}

	GroupHeaderView {
		width: parent.width;

		title: qsTr("General");
		groupView: generalGroup;
	}

	GroupElementView {
		id: generalGroup;

		width: parent.width;

		TextInputElementView {
			id: usernameInput_;

			name: qsTr("Username");
			placeHolderText: qsTr("Enter the username");
			readOnly: container.readOnly;
			showErrorWhenInvalid: true;
			errorText: qsTr("Please enter the username");
			textInputValidator: notEmptyRegexp;

			onEditingFinished: {
				let oldText = container.userData.m_username;
				if (oldText && oldText !== usernameInput_.text || !oldText && usernameInput_.text !== ""){

					container.emitUpdateModel();
				}
			}

			KeyNavigation.tab: passwordInput_;
		}

		RegularExpressionValidator {
			id: notEmptyRegexp;
			regularExpression: /^(?!\s*$).+/;
		}

		TextInputElementView {
			id: nameInput;

			name: qsTr("Name");
			placeHolderText: qsTr("Enter the name");
			readOnly: container.readOnly;
			showErrorWhenInvalid: true;
			errorText: qsTr("Please enter the name");
			textInputValidator: notEmptyRegexp;

			onEditingFinished: {
				let oldText = container.userData.m_name;
				if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
					container.emitUpdateModel();
				}
			}

			KeyNavigation.tab: mailInput;
			KeyNavigation.backtab: passwordInput_;
		}

		RegularExpressionValidator {
			id: mailValid;

			regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
		}

		TextInputElementView {
			id: mailInput;

			name: qsTr("Email Address");
			textInputValidator: mailValid;

			placeHolderText: qsTr("Enter the email");
			readOnly: container.readOnly;

			onEditingFinished: {
				container.emitUpdateModel();
			}

			KeyNavigation.tab: usernameInput_;
			KeyNavigation.backtab: nameInput;
		}

		function updateGui(){
			if (!container.userData){
				return;
			}

			usernameInput_.text = container.userData.m_username;
			nameInput.text = container.userData.m_name;
			mailInput.text = container.userData.m_email;
			passwordInput_.text = container.userData.m_password;
		}

		function updateModel(){
			if (!container.userData){
				return;
			}

			container.userData.m_username = usernameInput_.text;
			container.userData.m_name = nameInput.text;
			container.userData.m_email = mailInput.text;
			container.userData.m_password = passwordInput_.text;
		}
	}

	GroupElementView {
		id: passwordGroup;
		width: parent.width;

		Component {
			id: errorComp;

			Text {
				text: qsTr("Passwords don't match");
				color: Style.errorTextColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSize_common;
			}
		}

		Component {
			id: emptyPasswordErrorComp;

			Text {
				text: qsTr("Passwords don't match");
				color: Style.errorTextColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSize_common;
			}
		}

		Component {
			id: emptyComp;

			BaseText {
			}
		}

		function checkPassword(){
			if (passwordInput_.text === ""){
				confirmPassword.bottomComp = emptyPasswordErrorComp;
				return;
			}

			if (passwordInput_.text !== confirmPassword.text){
				confirmPassword.bottomComp = errorComp;

				return;
			}

			confirmPassword.bottomComp = emptyComp;

			let oldText = container.userData.m_password;
			if (oldText && oldText !== passwordInput_.text || !oldText && passwordInput_.text !== ""){
				container.emitUpdateModel();
			}
		}

		TextInputElementView {
			id: passwordInput_;
			name: qsTr("Password");
			placeHolderText: qsTr("Enter the password");
			echoMode: TextInput.Password;
			readOnly: container.readOnly;
			visible: passwordGroup.visible;
			onEditingFinished: {
				passwordGroup.checkPassword();
			}

			KeyNavigation.tab: confirmPassword;
			KeyNavigation.backtab: nameInput;
		}

		TextInputElementView {
			id: confirmPassword;
			name: qsTr("Confirm password");
			echoMode: TextInput.Password;
			placeHolderText: qsTr("Confirm password");
			readOnly: container.readOnly;
			visible: passwordInput_.visible;
			showErrorWhenInvalid: true;
			errorText: qsTr("Please enter the password");
			textInputValidator: notEmptyRegexp;
			onEditingFinished: {
				passwordGroup.checkPassword();
			}
			KeyNavigation.tab: usernameInput_;
			KeyNavigation.backtab: passwordInput_;
		}

		ElementView {
			id: changePasswordButton_;
			name: qsTr("Change password");
			visible: false;

			controlComp: Component {
				Button {
					width: 100;
					height: 30;
					text: qsTr("Change");
					onClicked: {
						ModalDialogManager.openDialog(changePasswordComp, {});
					}
				}
			}

			Component {
				id: changePasswordComp;
				ChangePasswordDialog {
					title: qsTr("Change Password");
					currentPasswordInputVisible: !AuthorizationController.loggedUserIsSuperuser();
					onFinished: {
						if (buttonId == Enums.save){
							AuthorizationController.changePassword(container.userData.m_username, contentItem.oldPassword, contentItem.newPassword);
						}
					}
				}
			}
		}
	}
}
