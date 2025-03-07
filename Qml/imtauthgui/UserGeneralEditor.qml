import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0
import imtdocgui 1.0

Column {
    id: container;
	spacing: Style.sizeLargeMargin;

	property UserData userData;
	property bool readOnly: false;

	signal emitUpdateModel();
	signal emitUpdateGui();

	property alias usernameInput: usernameInput_;
	property alias nameInput: nameInput_;
	property alias mailInput: mailInput_;
	property alias passwordInput: passwordInput_;
	property alias confirmPasswordInput: confirmPassword;
	property alias changePasswordButton: changePasswordButton_;

	property bool canHideGroup: true;

	function updateGui(){
		usernameInput_.text = container.userData.m_username;
		nameInput_.text = container.userData.m_name;
		mailInput_.text = container.userData.m_email;
		passwordInput_.text = container.userData.m_password;
	}

	function updateModel(){
		container.userData.m_username = usernameInput_.text;
		container.userData.m_name = nameInput_.text;
		container.userData.m_email = mailInput_.text;
		container.userData.m_password = passwordInput_.text;
	}

	// GroupHeaderView {
	// 	width: parent.width;

	// 	title: qsTr("General");
	// 	groupView: container.canHideGroup ? generalGroup : null;
	// }

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

			KeyNavigation.tab: nameInput_;
			KeyNavigation.backtab: passwordInput_.visible ? confirmPassword : mailInput_;
		}

		RegularExpressionValidator {
			id: notEmptyRegexp;
			regularExpression: /^(?!\s*$).+/;
		}

		TextInputElementView {
			id: nameInput_;

			name: qsTr("Name");
			placeHolderText: qsTr("Enter the name");
			readOnly: container.readOnly;
			showErrorWhenInvalid: true;
			errorText: qsTr("Please enter the name");
			textInputValidator: notEmptyRegexp;

			onEditingFinished: {
				let oldText = container.userData.m_name;
				if (oldText && oldText !== nameInput_.text || !oldText && nameInput_.text !== ""){
					container.emitUpdateModel();
				}
			}

			KeyNavigation.tab: mailInput_;
			KeyNavigation.backtab: usernameInput_;
		}

		RegularExpressionValidator {
			id: mailValid;

			regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
		}

		TextInputElementView {
			id: mailInput_;

			name: qsTr("Email Address");
			textInputValidator: mailValid;

			placeHolderText: qsTr("Enter the email");
			readOnly: container.readOnly;
			showErrorWhenInvalid: true;
			errorText: qsTr("Please enter the email");
			onEditingFinished: {
				container.emitUpdateModel();
			}

			KeyNavigation.tab: passwordInput_.visible ? passwordInput_ : usernameInput_;
			KeyNavigation.backtab: nameInput_;
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
				font.pixelSize: Style.fontSizeXSmall;
			}
		}

		Component {
			id: emptyPasswordErrorComp;

			Text {
				text: qsTr("Passwords don't match");
				color: Style.errorTextColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeXSmall;
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
			KeyNavigation.backtab: mailInput_;
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
