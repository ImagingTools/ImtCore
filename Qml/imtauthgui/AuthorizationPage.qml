import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Rectangle {
	id: authPageContainer;
	objectName: "AuthorizationPage"

	color: "transparent";

	property string state:"";

	property int mainRadius: Style.radiusL;
	property string mainColor: Style.backgroundColor2

	property bool canRecoveryPassword: true;
	property bool canRegisterUser: true;

	property string appName: context ? context.appName : ""
	
	property bool isAuthenticating: false;

	signal login(string login, string password)
	signal registerUser(var userData)

	property bool rememberMe: rememberMeCheckBox.checkState === Qt.Checked

	// Sync rememberMe with AuthorizationController
	Binding {
		target: AuthorizationController
		property: "rememberMe"
		value: authPageContainer.rememberMe
	}

	Component.onCompleted: {
		decoratorPause.start();

		authPageContainer.state = "unauthorized";
		authPageContainer.isAuthenticating = false;

		passwordTextInput.text = "";
		
		// Restore username from AuthorizationController if "Remember me" is checked
		if (authPageContainer.rememberMe && AuthorizationController.lastUser !== "") {
			loginTextInput.text = AuthorizationController.lastUser;
		}
		else {
			loginTextInput.text = "";
		}
		loginTextInput.forceActiveFocus();
	}

	LocalizationEvent {
		onLocalizationChanged: {
			authPageContainer.onLocalizationChanged(langId);
		}
	}

	function onLocalizationChanged(language){
		titleLogin.text = qsTr("Username")
	}

	onVisibleChanged: {
		if (authPageContainer.visible){
			loginTextInput.forceActiveFocus();
		}
	}

	Connections {
		target: AuthorizationController;

		function onLoginFailed(message){
			authPageContainer.isAuthenticating = false;
			// Clear password on error (common good UX: don't leave bad credentials; username stays)
			passwordTextInput.text = "";
			errorMessage.text = (message && message !== "") ? message : qsTr("Username or password is incorrect");
			passwordTextInput.forceActiveFocus();
		}

		function onLoggedIn(){
			authPageContainer.isAuthenticating = false;
		}
	}

	function passwordRecovery(){
		ModalDialogManager.openDialog(passwordRecoveryDialogComp, {});
	}

	function setDecorators(){
		if(inputDecoratorLoader1.item && inputDecoratorLoader1.item.rootItem){
			inputDecoratorLoader1.item.rootItem = loginTextInput;
		}
		if(titleDecoratorLoader2.item && inputDecoratorLoader2.item.rootItem){
			titleDecoratorLoader2.item.rootItem = titlePassword;
		}
		if(inputDecoratorLoader2.item && inputDecoratorLoader2.item.rootItem){
			inputDecoratorLoader2.item.rootItem = passwordTextInput;
		}
	}

	Rectangle{
		anchors.fill: parent;
		color: "gray";
		opacity: 0.5;
	}

	PauseAnimation {
		id: decoratorPause;

		duration: 500;

		onFinished: {
			authPageContainer.setDecorators();
			loginTextInput.forceActiveFocus();
		}
	}

	Component{
		id: emptyDecorator;
		Item{
			property Item rootItem: null;
		}
	}

	MouseArea {
		anchors.fill: parent;

		onClicked: {}
	}

	Rectangle {
		id: loginContainer;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.verticalCenter: parent.verticalCenter;

		width: Style.sizeHintM;
		//height: 380;
		height: bodyColumn.height + headerItem.height + Style.marginL;

		radius: authPageContainer.mainRadius;
		color: authPageContainer.mainColor;
		border.width: 1;
		border.color: Style.borderColor;

		clip: true;

		Component {
			id: headerDefaultComp;

			Rectangle{
				id: headerRec;

				width: loginContainer.width;
				height: 80;

				color: loginContainer.color;
				radius: loginContainer.radius;

				Text {
					id: welcomeText;

					anchors.top: parent.top;
					anchors.topMargin: 30;
					anchors.horizontalCenter: parent.horizontalCenter;

					color: Style.textColor;
					font.family: Style.fontFamily;
					font.pixelSize: Style.fontSizeXXL;

					text: authPageContainer.appName !== "" ? qsTr("Welcome to") + " " + authPageContainer.appName : qsTr("Welcome");
				}
			}
		}

		Item{
			id: headerItem;

			width: parent.width;
			height: 70;

			Loader{
				id: headerLoader;

				anchors.horizontalCenter: parent.horizontalCenter;

				sourceComponent: Style.authorizationHeaderDecorator !== undefined ? Style.authorizationHeaderDecorator: headerDefaultComp;

				onLoaded:{
					headerItem.height = headerLoader.item.height;
					headerLoader.width = headerLoader.item.width;
					headerLoader.height = headerLoader.item.height;
				}
			}
		}


		Column {
			id: bodyColumn;

			anchors.top: headerItem.bottom;
			anchors.topMargin: Style.marginL;
			anchors.horizontalCenter: parent.horizontalCenter;

			spacing: Style.marginL;

			Text {
				id: titleLogin;

				color: Style.textColor;
				font.family: Style.fontFamilyBold;
				font.pixelSize: Style.fontSizeM;

				text: qsTr("Username");

				Loader{
					id: titleDecoratorLoader1;

					sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
					onLoaded: {
					}
				}
			}

			CustomTextField {
				id: loginTextInput;
				objectName: "LoginInput"

				width: Style.sizeHintXS;
				height: Style.controlHeightM;

				readOnly: authPageContainer.isAuthenticating;
				placeHolderText: qsTr("Enter your username");
				KeyNavigation.tab: passwordTextInput;
				KeyNavigation.backtab: passwordTextInput;

				onTextChanged: {
					if (errorMessage.text != ""){
						errorMessage.text = "";
					}
				}

				onAccepted: {
					if (!authPageContainer.isAuthenticating && passwordTextInput.text != "" && loginTextInput.text != ""){
						loginButton.clicked();
					}
					else if(!authPageContainer.isAuthenticating && loginTextInput.text != ""){
						passwordTextInput.forceActiveFocus();
					}
				}

				Loader{
					id: inputDecoratorLoader1;

					sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
					onLoaded: {
					}
				}
			}

			Text {
				id: titlePassword;

				color: Style.textColor;
				font.family: Style.fontFamilyBold;
				font.pixelSize: Style.fontSizeM;

				text: qsTr("Password");

				Loader{
					id: titleDecoratorLoader2;

					sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
					onLoaded: {
					}
				}
			}

			CustomTextField {
				id: passwordTextInput;
				objectName: "PasswordInput"

				width: Style.sizeHintXS;
				height: Style.controlHeightM;

				readOnly: authPageContainer.isAuthenticating;
				placeHolderText: qsTr("Enter your password");
				echoMode: TextInput.Password;

				KeyNavigation.tab: loginTextInput;
				KeyNavigation.backtab: loginTextInput;

				onTextChanged: {
					if (errorMessage.text != ""){
						errorMessage.text = "";
					}
				}

				onAccepted: {
					if (!authPageContainer.isAuthenticating && passwordTextInput.text != "" && loginTextInput.text != ""){
						loginButton.clicked();
					}
				}

				Loader{
					id: inputDecoratorLoader2;

					sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
					onLoaded: {

					}
				}

				ToolButton {
					id: eyeButton;
					objectName: "EyeButton"

					anchors.verticalCenter: parent.verticalCenter;
					anchors.right: parent.right;
					anchors.rightMargin: 4;

					height: Math.min(24, parent.height - 10);
					width: height;
					z: 20
					enabled: !authPageContainer.isAuthenticating

					iconSource: passwordTextInput.echoMode == TextInput.Password ? "../../../" + Style.getIconPath("Icons/ShownPassword", Icon.State.On, Icon.Mode.Normal) :
																				   "../../../" + Style.getIconPath("Icons/HiddenPassword", Icon.State.On, Icon.Mode.Normal) ;

					onClicked: {
						if(passwordTextInput.echoMode == TextInput.Password){
							passwordTextInput.echoMode = TextInput.Normal;
						}
						else if(passwordTextInput.echoMode == TextInput.Normal){
							passwordTextInput.echoMode = TextInput.Password;
						}
					}
				}
			}

			// Modern compact row: Remember me (left) + Password recovery (right)
			// This is a common clean pattern in contemporary login forms.
			Item {
				id: optionsRow;
				width: parent.width;
				height: rememberMeCheckBox.height + 2;
				visible: true;   // remember is always present; recovery visibility is handled on children

				CheckBox {
					id: rememberMeCheckBox;
					objectName: "RememberMeCheckBox"
					anchors.left: parent.left;
					anchors.verticalCenter: parent.verticalCenter;
					checkState: Qt.Checked;
					enabled: !authPageContainer.isAuthenticating;

					text: qsTr("Remember me");
				}

				Text {
					id: titlePasswordRecovery;

					anchors.right: parent.right;
					anchors.verticalCenter: parent.verticalCenter;
					visible: authPageContainer.canRecoveryPassword;

					color: Style.linkColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeM;
					font.underline: true;

					text: qsTr("Forgot password?");

					Loader{
						id: titleDecoratorLoader3;

						sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
						onLoaded: {
						}
					}
				}

				MouseArea{
					id: passwordRecoveryMA;

					anchors.fill: titlePasswordRecovery;
					visible: authPageContainer.canRecoveryPassword;
					enabled: !authPageContainer.isAuthenticating;
					cursorShape: Qt.PointingHandCursor;
					hoverEnabled: true;
					onClicked: {
						authPageContainer.passwordRecovery();
					}
				}
			}

			Item {
				id: buttonItem;

				width: parent.width;
				height: buttonContent.height + Style.marginL;
				// Extra bottom space so the Sign in button is not pressed against the bottom of the card.

				Column {
					id: buttonContent;
					anchors.centerIn: parent;
					spacing: Style.marginM;
					width: parent.width;

					Button {
						id: loginButton;
						objectName: "LoginButton"
						anchors.horizontalCenter: parent.horizontalCenter;

						width: Style.buttonWidthXXL;
						height: Style.controlHeightM;

						enabled: !authPageContainer.isAuthenticating && loginTextInput.text != "" && passwordTextInput.text != "";

						text: qsTr("Sign in");

						onClicked: {
							errorMessage.text = "";
							authPageContainer.isAuthenticating = true;
							authPageContainer.login(loginTextInput.text, passwordTextInput.text)
						}

						// Loading indicator moved inside the button for better UX
						Loading {
							id: loginLoadingIndicator;
							visible: authPageContainer.isAuthenticating;
							anchors.right: parent.right;
							anchors.rightMargin: 8;
							anchors.verticalCenter: parent.verticalCenter;
							width: 18;
							height: 18;
							color: "transparent";
							background.color: "transparent";
							indicatorSize: 14;
							z: 20;
						}
					}

					// Fixed-height container for error placed right under the Sign in button.
					// The reserved space ensures the login card height never jumps when an error is shown/hidden.
					Item {
						id: errorContainer;
						width: parent.width;
						height: Style.controlHeightS + Style.marginS;

						Text {
							id: errorMessage;

							anchors.top: parent.top;
							anchors.topMargin: 2;
							anchors.horizontalCenter: parent.horizontalCenter;
							width: parent.width;

							color: Style.errorTextColor;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;

							visible: text !== "";
							horizontalAlignment: Text.AlignHCenter;
							wrapMode: Text.Wrap;
						}
					}
				}
			}// buttonItem

			// Register link placed below the main button - cleaner modern pattern
			// (less clutter above the primary action).
			Item {
				id: registerItem;
				width: parent.width;
				height: registerUserText.height + Style.marginM;
				visible: authPageContainer.canRegisterUser;

				Text {
					id: registerUserText;

					anchors.horizontalCenter: parent.horizontalCenter;
					anchors.bottom: parent.bottom;

					color: Style.linkColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeM;
					font.underline: true;

					text: qsTr("Sign up");

					Loader{
						sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
						onLoaded: {
						}
					}
				}

				MouseArea{
					anchors.fill: registerUserText;
					objectName: "RegisterUser"
					enabled: !authPageContainer.isAuthenticating;
					cursorShape: Qt.PointingHandCursor;
					hoverEnabled: true;
					onClicked: {
						ModalDialogManager.openDialog(registerDialogComp, {});
					}
				}
			}

			// Small bottom spacer so the last element (button or Register) never feels
			// pressed against the bottom edge of the card. Modern cards need breathing room.
			Item {
				width: 1;
				height: Style.marginM;
			}
		}//bodyColumn
	}

	Component{
		id: passwordRecoveryDialogComp;
		PasswordRecoveryDialog {}
	}

	Component {
		id: registerDialogComp;
		Dialog {
			id: registerDialog;
			width: Style.sizeHintXXL;
			height: ModalDialogManager.activeView.height - 100;
			title: qsTr("Sign up");
			canMove: false;

			UserData {
				id: userDataModel;
				m_id: UuidGenerator.generateUUID();
			}

			Component.onCompleted: {
				addButton(Enums.save, qsTr("Sign up"), true);
				addButton(Enums.close, qsTr("Close"), true);
			}

			contentComp: Component {
				Item {
					width: registerDialog.width;
					height: registerDialog.height - 100;

					clip: true;

					CustomScrollbar {
						id: scrollbar;

						z: parent.z + 1;

						anchors.right: parent.right;
						anchors.top: flickable.top;
						anchors.bottom: flickable.bottom;

						secondSize: 10;
						targetItem: flickable;

						radius: 2;
					}

					Flickable {
						id: flickable;
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginXL;
						anchors.right: parent.right
						anchors.rightMargin: Style.marginXL;
						height: parent.height;

						contentHeight: userGeneralEditor.height;
						contentWidth: width;

						UserGeneralEditor {
							id: userGeneralEditor;
							width: flickable.width;
							userData: userDataModel;

							onEmitUpdateModel: {
								userGeneralEditor.updateModel();
							}
						}
					}
				}
			}

			onFinished: {
				if (buttonId === Enums.save){
					authPageContainer.registerUser(userDataModel)
				}
			}
		}
	}
}
