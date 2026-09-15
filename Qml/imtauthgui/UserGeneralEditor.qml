import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Column {
	id: container;
	spacing: Style.marginXL;

	property UserData userData;
	property bool readOnly: false;

	// PasswordPolicyController instance, injected by the owning view.
	property var passwordPolicy: null;

	signal emitUpdateModel();
	signal emitUpdateGui();

	property alias usernameInput: usernameInput_;
	property alias nameInput: nameInput_;
	property alias mailInput: mailInput_;
	property alias passwordInput: passwordInput_;
	property alias confirmPasswordInput: confirmPassword;
	property alias changePasswordButton: changePasswordButton_;

	property bool canHideGroup: true;

	property bool showAccountEnabled: false;

	// Mirrors the server side guard: only the superuser may enable or disable an account,
	// so for everyone else the switch is absent and the model keeps the stored state.
	readonly property bool accountEnabledAvailable: container.showAccountEnabled && AuthorizationController.loggedUserIsSuperuser();

	property bool updatingGui: false;

	function updateGui(){
		if (!container.userData){
			return
		}

		container.updatingGui = true;

		usernameInput_.text = container.userData.m_username;
		nameInput_.text = container.userData.m_name;
		mailInput_.text = container.userData.m_email;
		passwordInput_.text = container.userData.m_password;
		if (container.accountEnabledAvailable){
			enabledSwitch_.checked = container.userData.m_enabled === false ? false : true;
		}

		container.updatingGui = false;
	}

	function updateModel(){
		if (!container.userData){
			return
		}

		container.userData.m_username = usernameInput_.text;
		container.userData.m_name = nameInput_.text;
		container.userData.m_email = mailInput_.text;
		container.userData.m_password = passwordInput_.text;
		if (container.accountEnabledAvailable){
			container.userData.m_enabled = enabledSwitch_.checked;
		}
	}

	GroupElementView {
		id: generalGroup;

		width: parent.width;

		TextInputElementView {
			id: usernameInput_;

			// Test instrumentation: matches the AccountEditor/DeviceEditor/etc. convention of an
			// explicit per-field objectName on the ElementView usage site. Inert.
			objectName: "UsernameInput";

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

			// Test instrumentation - see usernameInput_'s comment above. Inert.
			objectName: "UserNameInput";

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

			// Test instrumentation - see usernameInput_'s comment above. Inert.
			objectName: "MailInput";

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

		SwitchElementView {
			id: enabledSwitch_;

			// Test instrumentation - see usernameInput_'s comment above. Inert.
			objectName: "AccountEnabledSwitch";

			name: qsTr("Account enabled");
			description: qsTr("Disabled accounts cannot log in");
			visible: container.accountEnabledAvailable;
			readOnly: container.readOnly;

			onCheckedChanged: {
				if (container.updatingGui){
					return;
				}

				container.emitUpdateModel();
			}
		}
	}

	GroupElementView {
		id: passwordGroup;
		width: parent.width;

		property string policyErrorText: "";

		Component.onCompleted: {
			if (container.passwordPolicy){
				container.passwordPolicy.load();
			}
		}

		Component {
			id: policyErrorComp;

			Text {
				text: passwordGroup.policyErrorText;
				wrapMode: Text.WordWrap;
				color: Style.errorTextColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
			}
		}

		Component {
			id: errorComp;

			Text {
				text: qsTr("Passwords don't match");
				color: Style.errorTextColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
			}
		}

		Component {
			id: emptyPasswordErrorComp;

			Text {
				text: qsTr("Passwords don't match");
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

		function checkPassword(){
			if (passwordInput_.text === ""){
				confirmPassword.bottomComp = emptyPasswordErrorComp;
				return;
			}

			if (passwordInput_.text !== confirmPassword.text){
				confirmPassword.bottomComp = errorComp;

				return;
			}

			let login = container.userData ? container.userData.m_username : "";
			let violatedRules = container.passwordPolicy ? container.passwordPolicy.validate(login, passwordInput_.text) : [];
			if (violatedRules.length > 0){
				passwordGroup.policyErrorText = container.passwordPolicy.describeFailure(violatedRules, "");
				confirmPassword.bottomComp = policyErrorComp;

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
			// Test instrumentation - see usernameInput_'s comment above. Inert.
			objectName: "PasswordInput";
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
			// Test instrumentation - see usernameInput_'s comment above. Inert.
			objectName: "ConfirmPasswordInput";
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
					width: Style.buttonWidthXXL;
					height: Style.controlHeightM;
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
					login: container.userData ? container.userData.m_username : "";
					policy: container.passwordPolicy;
					onFinished: {
						if (buttonId == Enums.save){
							passwordChangeConnections.enabled = true;
							AuthorizationController.changePassword(container.userData.m_username, contentItem.oldPassword, contentItem.newPassword);
						}
					}
				}
			}

			Connections {
				id: passwordChangeConnections;
				target: AuthorizationController;
				enabled: false;

				function onChangePasswordFailed(message, violatedRules) {
					passwordChangeConnections.enabled = false;

					let fallback = qsTr("Unable to change the password.");
					PopupManager.addErrorMessage(
								container.passwordPolicy
									? container.passwordPolicy.describeFailure(violatedRules, fallback)
									: fallback,
								true);
				}

				function onChangePasswordSuccessfully() {
					passwordChangeConnections.enabled = false;
				}
			}
		}
	}
}
