import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: authPageContainer;

    color: Style.dialogBackgroundColor;
    property var windows;

    property string state;

    property alias tokenProvider: userTokenProvider;

    property int mainRadius: 3;
    property string mainColor: Style.backgroundColor;

    onVisibleChanged: {
        if (visible){
            authPageContainer.state = "unauthorized";

            passwordTextInput.text = "";
            loginTextInput.text = ""

            loginTextInput.focus = true;
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem;
        }
    }

    Component{
        id: defaultButton;

        AuxButton {
            width: 100;
            height: 30;

            hasText: true;
            hasIcon: false;

            color: Style.buttonColor;
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

        width: 400;
        height: 350;

        radius: authPageContainer.mainRadius;
        color: authPageContainer.mainColor;

        clip: true;

        Rectangle{
            id: headerRec;

            width: parent.width;
            height: 80;

            color: parent.color;
            radius: parent.radius;

            Text{
                id: welcomeText;

                anchors.top: parent.top;
                anchors.topMargin: 30;
                anchors.left: parent.left;
                anchors.leftMargin: (parent.width-welcomeText.width)/2;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_title;

                text: qsTr("Welcome");
            }

            //            Text{
            //                id: loginText;

            //                anchors.top: welcomeText.bottom;
            //                anchors.topMargin: 20;
            //                anchors.left: parent.left;
            //                anchors.leftMargin: (parent.width-loginText.width)/2;

            //                color: Style.textColor;
            //                font.family: Style.fontFamily;
            //                font.pixelSize: Style.fontSize_common;
            //                text: qsTr("Please login");
            //            }
        }

        Column {
            id: bodyColumn;

            anchors.top: headerRec.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;

            spacing: 10;

            Text {
                id: titleLogin;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Login");

                Loader{
                    id: titleDecoratorLoader1;

                    sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                    onLoaded: {
                        if(titleDecoratorLoader1.item){
                            titleDecoratorLoader1.item.rootItem = titleLogin;
                        }
                    }
                }
            }

            CustomTextField {
                id: loginTextInput;

                width: 300;
                height: 30;

                placeHolderText: qsTr("Enter the login");
                KeyNavigation.tab: passwordTextInput;

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
                    id: inputDecoratorLoader1;

                    sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                    onLoaded: {
                        if(inputDecoratorLoader1.item){
                            inputDecoratorLoader1.item.rootItem = loginTextInput;
                        }
                    }
                }
            }

            Text {
                id: titlePassword;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Password");

                Loader{
                    id: titleDecoratorLoader2;

                    sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                    onLoaded: {
                        if(titleDecoratorLoader2.item){
                            titleDecoratorLoader2.item.rootItem = titlePassword;
                        }
                    }
                }
            }

            CustomTextField {
                id: passwordTextInput;

                width: 300;
                height: 30;

                placeHolderText: qsTr("Enter the password");
                echoMode: TextInput.Password

                KeyNavigation.tab: loginTextInput;

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
                        if(inputDecoratorLoader2.item){
                            inputDecoratorLoader2.item.rootItem = passwordTextInput;
                        }
                    }
                }
            }

            Item{
                id: errorMessageItem;

                width: parent.width;
                height: 20;

                Text {
                    id: errorMessage;

                    anchors.verticalCenter: parent.verticalCenter;

                    color:  Style.errorTextColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    visible:  errorMessage.text != "";
                }

            }


            //            Item {
            //                id: remember;

            //                width: parent.width;
            //                height: 40;

            //                CheckBox {
            //                    id: checkRemember;

            //                    anchors.verticalCenter: parent.verticalCenter;

            //                    checkState: Qt.Unchecked;

            //                    onClicked: {
            //                        if (checkRemember.checkState == Qt.Unchecked){
            //                            checkRemember.checkState = Qt.Checked
            //                        }
            //                        else{
            //                           checkRemember.checkState = Qt.Unchecked
            //                        }
            //                    }
            //                }

            //                Text {
            //                    id: rememberText;

            //                    anchors.left: checkRemember.right;
            //                    anchors.leftMargin: 7;
            //                    anchors.verticalCenter: parent.verticalCenter;

            //                    color: Style.textColor;
            //                    font.family: Style.fontFamily;
            //                    font.pixelSize: Style.fontSize_small;
            //                    text: qsTr("Remember me");
            //                }
            //                Text{
            //                    id: forgotText;

            //                    anchors.top: parent.top;
            //                    anchors.topMargin: 7;
            //                    anchors.left: rememberText.right;
            //                    anchors.leftMargin: 120;

            //                    color: Style.textColor;
            //                    font.family: Style.fontFamily;
            //                    font.pixelSize: Style.fontSize_small;
            //                    //            text: '<html><style type="text/css"></style><a href="http://google.com">Forgot password?</a></html>'
            //                    onLinkActivated: {
            //                        Qt.openUrlExternally(link)
            //                    }
            //                }
            //            }

            Item {
                id: buttonItem;

                width: parent.width;
                height: 70;

                Item {
                    id: loginButton;

                    anchors.centerIn: parent;

                    width: parent.width;
                    height: buttonLoader.item.height !== undefined ? buttonLoader.item.height : 30;

                    enabled: loginTextInput.text != "" && passwordTextInput.text != "";

                    property string text:  qsTr("Login");

                    signal clicked();

                    onTextChanged: {
                        if(buttonLoader.item.textButton !==undefined){
                            buttonLoader.item.textButton = loginButton.text;
                        }
                    }
                    onEnabledChanged: {
                        if(buttonLoader.item){
                            buttonLoader.item.enabled = loginButton.enabled;

                        }
                    }

                    onClicked: {
                        userTokenProvider.authorization(loginTextInput.text, passwordTextInput.text);
                    }

                    Loader{
                        id: buttonLoader;

                        anchors.centerIn:  parent;
                        sourceComponent:  Style.dialogButtonComponent !== undefined ? Style.dialogButtonComponent: defaultButton;

                        onLoaded: {
                            if(buttonLoader.item){
                                buttonLoader.item.clicked.connect(loginButton.clicked);
                                buttonLoader.item.textButton = loginButton.text;
                                buttonLoader.item.enabled = loginButton.enabled;
                            }
                        }
                    }
                }
            }
        }
    }

    UserTokenProvider {
        id: userTokenProvider;

        onAccepted: {
            console.log('DEBUG::UserTokenProvider.onAccepted', authPageContainer.windows)
            authPageContainer.state = "authorized";
            var pageContainer =  authPageContainer.windows;
            pageContainer.updateAllModels();
            authPageContainer.visible = false;
        }
    }
}
