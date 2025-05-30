import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0
import imtguigql 1.0

Dialog {
	id: passwordRecoveryDialog;

	width: Style.sizeHintXXL;
	height: Style.sizeHintL;

	title: qsTr("Password Recovery");
	canMove: false;
	notClosingButtons: Enums.yes;

	property int currentIndex: -1;

	property string login: "";
	property string username: "";
	property string email: "";
	property string code: "";
	property string newPassword: "";
	property int defaultSecs: 60;
	property int secs: defaultSecs;

	signal refreshSendButton();

	onCurrentIndexChanged: {
		if (!stackView){
			return;
		}

		stackView.clear();
		if (currentIndex === -1){
			stackView.push(loadingPageComp);
		}
		else if (currentIndex === 0){
			passwordRecoveryDialog.setButtonEnabled(Enums.yes, false);
			passwordRecoveryDialog.setButtonName(Enums.yes, qsTr("Check the email"));

			stackView.push(pageComp0);
		}
		else if (currentIndex === 1){
			passwordRecoveryDialog.setButtonName(Enums.yes, qsTr("Yes"));

			stackView.push(pageComp1);
		}
		else if (currentIndex === 2){
			passwordRecoveryDialog.setButtonEnabled(Enums.yes, false);
			passwordRecoveryDialog.setButtonName(Enums.yes, qsTr("Check the code"));

			stackView.push(pageComp2);
		}
		else if (currentIndex === 3){
			passwordRecoveryDialog.setButtonEnabled(Enums.yes, false);
			passwordRecoveryDialog.setButtonName(Enums.yes, qsTr("Change password"));
			stackView.push(pageComp3);
		}
	}

	property StackView stackView: null;

	contentComp: Component {
		Item {
			width: passwordRecoveryDialog.width;
			height: passwordRecoveryDialog.height - 40;
			StackView {
				id: stackView_;
				anchors.fill: parent;
				Component.onCompleted: {
					passwordRecoveryDialog.stackView = stackView_;
					passwordRecoveryDialog.currentIndex = 0;
				}
			}
		}
	}

	Component.onCompleted: {
		addButton(Enums.yes, qsTr("Check the email"), false);
		addButton(Enums.cancel, qsTr("Cancel"), true);
	}

	onFinished: {
		if (buttonId === Enums.yes){
			if (currentIndex == 0){
				checkEmailRequestSender.send();
			}
			else if (currentIndex == 1){
				sendEmailCodeRequestSender.send();
				currentIndex = 2;

				return;
			}
			else if (currentIndex == 2){
				checkEmailCodeRequestSender.send();
			}
			else if (currentIndex == 3){
				changePasswordRequestSender.send();
			}

			currentIndex = -1;
		}
		else if (buttonId === Enums.no){
			ModalDialogManager.showInfoDialog(qsTr("Check the email you entered"));

			currentIndex == 0;
			finished(Enums.cancel);
		}
	}

	Component {
		id: loadingPageComp;

		Loading {
			anchors.fill: parent;
		}
	}

	Component {
		id: pageComp0;

		Item {
			id: firstPage;

			function checkNextButtonState(){
				passwordRecoveryDialog.setButtonEnabled(Enums.yes, emailElementView.acceptableInput)
			}

			Component.onCompleted: {
				emailElementView.forceActiveFocus();
			}

			GroupElementView {
				id: group;

				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginXL;

				TextInputElementView {
					id: emailElementView;
					placeHolderText: qsTr("Enter the email");
					name: qsTr("Email");
					description: qsTr("Enter the email address that was specified on your account, a code will be sent to it");
					text: passwordRecoveryDialog.email;

					textInputValidator: MailRegExpValidator {
						id: mailRegExpValidator;
					}
					showErrorWhenInvalid: true;
					errorText: qsTr("Please enter the valid email");

					onTextChanged: {
						passwordRecoveryDialog.email = text;

						firstPage.checkNextButtonState();
					}
				}
			}
		}
	}

	Component {
		id: pageComp2;

		Item {
			id: secondPage;

			function checkNextButtonState(){
				passwordRecoveryDialog.setButtonEnabled(Enums.yes, codeElementView.acceptableInput)
			}

			Component.onCompleted: {
				codeElementView.forceActiveFocus();
			}

			GroupElementView {
				id: group;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginXL;

				TextInputElementView {
					id: codeElementView;
					placeHolderText: qsTr("Enter the code");
					name: qsTr("Code");
					description: qsTr("Please enter the code sent to your email");
					text: passwordRecoveryDialog.code;
					onTextChanged: {
						passwordRecoveryDialog.code = text;

						secondPage.checkNextButtonState();
					}
				}

				ButtonElementView {
					id: button;
					name: qsTr("Send the code again");
					text: qsTr("Send");
					description: passwordRecoveryDialog.secs == 0 ? "" : qsTr("The new code can be obtained via") + " " + passwordRecoveryDialog.secs + " secs";
					buttonEnabled: passwordRecoveryDialog.secs == 0;

					onClicked: {
						sendEmailCodeRequestSender.send()
					}
				}

				Timer {
					id: timer;
					interval: 1000;
					running: passwordRecoveryDialog.secs > 0;
					repeat: true;

					onTriggered: {
						console.log("onTriggered");
						if (passwordRecoveryDialog.secs > 0){
							passwordRecoveryDialog.secs--;
						}
					}
				}

				Connections {
					target: passwordRecoveryDialog;

					function onRefreshSendButton(){
						passwordRecoveryDialog.secs = passwordRecoveryDialog.defaultSecs;
					}
				}
			}
		}
	}

	Component {
		id: pageComp1;

		Item {
			GroupElementView {
				id: group;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginXL;

				TextInputElementView {
					id: usernameElementView;
					name: qsTr("Username");
					description: qsTr("For this email this account has been found, is that you?");
					text: passwordRecoveryDialog.username;
					readOnly: true;
				}
			}
		}
	}

	Component {
		id: pageComp3;

		Item {
			PasswordInput {
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginXL;
				currentPasswordInputVisible: false;
				onAcceptedChanged: {
					passwordRecoveryDialog.setButtonEnabled(Enums.yes, accepted);
				}
				onNewPasswordChanged: {
					passwordRecoveryDialog.newPassword = newPassword;
				}
			}
		}
	}

	GqlSdlRequestSender {
		id: checkEmailRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_checkEmail;

		inputObjectComp: Component {
			CheckEmailInput {
				m_email: passwordRecoveryDialog.email;
			}
		}

		sdlObjectComp: Component {
			CheckEmailPayload {
				onFinished: {
					if (m_success){
						passwordRecoveryDialog.username = m_userName;
						passwordRecoveryDialog.login = m_login;
					}
					else{
						if (m_message !== ""){
							ModalDialogManager.showErrorDialog(m_message);
						}
					}

					passwordRecoveryDialog.currentIndex = m_success ? 1 : 0;
				}
			}
		}

		onFinished: {
			if (status < 0){
				passwordRecoveryDialog.currentIndex = 0;
			}
		}
	}

	GqlSdlRequestSender {
		id: checkEmailCodeRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_checkEmailCode;

		inputObjectComp: Component {
			CheckEmailCodeInput {
				m_login: passwordRecoveryDialog.login;
				m_code: passwordRecoveryDialog.code;
			}
		}

		sdlObjectComp: Component {
			CheckEmailCodePayload {
				onFinished: {
					passwordRecoveryDialog.currentIndex = m_correctCode ? 3 : 2;
				}
			}
		}

		onFinished: {
			if (status < 0){
				passwordRecoveryDialog.currentIndex = 2;
			}
		}
	}

	GqlSdlRequestSender {
		id: sendEmailCodeRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_sendEmailCode;

		inputObjectComp: Component {
			SendEmailCodeInput {
				m_login: passwordRecoveryDialog.login;
			}
		}
		sdlObjectComp: Component {
			SendEmailCodePayload {
				onFinished: {
					if (m_success){
						passwordRecoveryDialog.refreshSendButton();
					}
					else{
						if (m_message !== ""){
							ModalDialogManager.showErrorDialog(m_message);
						}
					}
				}
			}
		}
	}

	GqlSdlRequestSender {
		id: changePasswordRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_changePassword;

		inputObjectComp: Component {
			ChangePasswordInput {
				m_login: passwordRecoveryDialog.login;
				m_newPassword: passwordRecoveryDialog.newPassword;
			}
		}

		sdlObjectComp: Component {
			ChangePasswordPayload {
				onFinished: {
					if (m_success){
						passwordRecoveryDialog.finished(Enums.cancel)
					}
				}
			}
		}

		onFinished: {
			if (status < 0){
				passwordRecoveryDialog.currentIndex = 2;
			}
			else{
				ModalDialogManager.showInfoDialog(qsTr("Password changed successfully"));
			}
		}
	}
}
