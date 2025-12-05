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

	property int mainRadius: Style.radiusS;
	property string mainColor: Style.backgroundColor;

	property bool canRecoveryPassword: true;
	property bool canRegisterUser: true;

	property string appName: context ? context.appName : ""
	
	signal login(string login, string password)
	signal registerUser(var userData)

	Component.onCompleted: {
		decoratorPause.start();
	}

	LocalizationEvent {
		onLocalizationChanged: {
			authPageContainer.onLocalizationChanged(langId);
		}
	}

	function onLocalizationChanged(language){
		titleLogin.text = qsTr("Login")
	}

	onVisibleChanged: {
		if (authPageContainer.visible){
			authPageContainer.state = "unauthorized";

			passwordTextInput.text = "";
			loginTextInput.text = "";

			loginTextInput.forceActiveFocus()
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
		height: bodyColumn.height + headerItem.height;

		radius: authPageContainer.mainRadius;
		color: authPageContainer.mainColor;

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
					anchors.left: parent.left;
					anchors.leftMargin: (parent.width - welcomeText.width)/2;

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
			anchors.horizontalCenter: parent.horizontalCenter;

			spacing: Style.marginM;

			Text {
				id: titleLogin;

				color: Style.textColor;
				font.family: Style.fontFamilyBold;
				font.pixelSize: Style.fontSizeM;

				text: qsTr("Login");

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

				placeHolderText: qsTr("Enter the login");
				KeyNavigation.tab: passwordTextInput;
				KeyNavigation.backtab: passwordTextInput;

				onTextChanged: {
					if (errorMessage.text != ""){
						errorMessage.text = "";
					}
				}

				onAccepted: {
					if (passwordTextInput.text != "" && loginTextInput.text != ""){
						loginButton.clicked();
					}
					else if(loginTextInput.text != ""){
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

				placeHolderText: qsTr("Enter the password");
				echoMode: TextInput.Password;

				KeyNavigation.tab: loginTextInput;
				KeyNavigation.backtab: loginTextInput;

				onTextChanged: {
					if (errorMessage.text != ""){
						errorMessage.text = "";
					}
				}

				onAccepted: {
					if (passwordTextInput.text != "" && loginTextInput.text != ""){
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

			Item{
				id: passwordRecoveryItem;

				width: parent.width;
				height: titlePasswordRecovery.height;

				visible: authPageContainer.canRecoveryPassword;

				Text {
					id: titlePasswordRecovery;

					anchors.right: parent.right;

					color: Style.textColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeM;
					font.underline: true;

					text: qsTr("Password recovery");

					Loader{
						id: titleDecoratorLoader3;

						sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
						onLoaded: {
						}
					}
				}

				MouseArea{
					id: passwordRecoveryMA;

					anchors.fill: parent;

					cursorShape: Qt.PointingHandCursor;
					hoverEnabled: true;
					onClicked: {
						authPageContainer.passwordRecovery();
					}
				}
			}

			Item{
				id: registerItem;
				width: parent.width;
				height: registerUserText.height;
				visible: authPageContainer.canRegisterUser;

				Text {
					id: registerUserText;

					anchors.right: parent.right;

					color: Style.textColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeM;
					font.underline: true;

					text: qsTr("Register");

					Loader{
						sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
						onLoaded: {
						}
					}
				}

				MouseArea{
					anchors.fill: registerUserText;

					cursorShape: Qt.PointingHandCursor;
					hoverEnabled: true;
					onClicked: {
						ModalDialogManager.openDialog(registerDialogComp, {});
					}
				}
			}

			Item{
				id: errorMessageItem;

				width: parent.width;
				height: Style.controlHeightS;

				Text {
					id: errorMessage;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.horizontalCenter: parent.horizontalCenter;

					color:  Style.errorTextColor;
					font.family: Style.fontFamily;
					font.pixelSize: Style.fontSizeM;

					visible:  errorMessage.text != "";
				}
			}

			Item {
				id: buttonItem;

				width: parent.width;
				height: 70;

				Button{
					id: loginButton;
					objectName: "LoginButton"
					
					anchors.centerIn: parent;
					width: Style.buttonWidthXXL;
					height: Style.controlHeightM;

					property int loadingCount: 0;//for web
					function setItemPropertiesAdd(){
						if(loginButton.loadingCount > 0){
							loginButton.width = 0;
						}
					}

					enabled: loginTextInput.text != "" && passwordTextInput.text != "";

					text: qsTr("Login");

					onClicked: {
						authPageContainer.login(loginTextInput.text, passwordTextInput.text)
					}
				}
			}//
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
			title: qsTr("User Registration");
			canMove: false;

			UserData {
				id: userDataModel;
				m_id: UuidGenerator.generateUUID();
			}

			Component.onCompleted: {
				addButton(Enums.save, qsTr("Register"), true);
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
