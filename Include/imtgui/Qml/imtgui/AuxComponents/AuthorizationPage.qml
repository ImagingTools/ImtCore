import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: authPageContainer;

    color: Style.dialogBackgroundColor;

    property alias tokenProvider: userTokenProvider;

    MouseArea {
        anchors.fill: parent;

        onClicked: {}
    }

    Rectangle {
        id: loginContainer;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        width: 450;
        height: 380;

        radius: 3;
        clip: true;
        color: Style.backgroundColor;

        Rectangle{
            id: headerRec;

            width: parent.width;
            height: 100;

            color: Style.backgroundColor;

            Text{
                id: welcomeText;

                anchors.top: parent.top;
                anchors.topMargin: 30;
                anchors.left: parent.left;
                anchors.leftMargin: (parent.width-welcomeText.width)/2;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_title;
                text: qsTr("Welcome to Lisa");
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

            spacing: 7;

            Text {
                id: titleLogin;

                text: qsTr("Login");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
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
            }

            Text {
                id: titlePassword;

                text: qsTr("Password");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
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
            }

            Text {
                id: errorMessage;

                color: Style.errorTextColor;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                visible: errorMessage.text != "";
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
                 height: 100;

                 AuxButton {
                     id: loginButton;

                     anchors.horizontalCenter: parent.horizontalCenter;
                     anchors.verticalCenter: parent.verticalCenter;

                     width: 100;
                     height: 30;

                     hasText: true;
                     textButton: qsTr("Login");
                     color: Style.buttonColor;

                     enabled: loginTextInput.text != "" && passwordTextInput.text != "";

                     onClicked: {
                         userTokenProvider.authorization(loginTextInput.text, passwordTextInput.text);
                     }
                 }
             }
        }
    }

    UserTokenProvider {
        id: userTokenProvider;

        onAccepted: {
            console.log('DEBUG::UserTokenProvider.onAccepted')
            window.updateAllModels();
            authPageContainer.visible = false;
        }
    }
}
