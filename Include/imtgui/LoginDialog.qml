import QtQuick 2.0
import QtQuick 2.15
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import QtQuick.Dialogs 1.3
import imtauthgui 1.0

Rectangle {
    id: loginContainer;

    width: 450;
    height: 380;

    radius: 3;

    clip: true;
    visible: false;

    color: Style.backgroundColor;

    property real backgroundOpacity: 0.3;
    property bool centered: true;

    property Item loaderDialog;
    property Item thumbnailItem;

    property TreeItemModel localSettings;
    property TreeItemModel serverSettings;


    Component.onCompleted: {
        console.log("LoginDialog onCompleted");
    }

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
        Text{
            id: loginText;
            anchors.top: welcomeText.bottom;
            anchors.topMargin: 20;
            anchors.left: parent.left;
            anchors.leftMargin: (parent.width-loginText.width)/2;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            text: qsTr("Please login");
        }
    }
    TextFieldCustom {
        id: loginTextInput;
        anchors.left: parent.left;
        anchors.top: headerRec.bottom;
        anchors.topMargin: 50;
        anchors.leftMargin: 75;
        width: loginContainer.width - 150;
        height: 30;

        onInputTextChanged: {
            console.log("LoginTextInput onInputTextChanged");
        }
    }
    TextFieldCustom {
        id: passwordTextInput;
        anchors.left: parent.left;
        anchors.top: loginTextInput.bottom;
        anchors.topMargin: 30;
        anchors.leftMargin: 75;
        width: loginContainer.width - 150;
        height: 30;
        echoMode: TextInput.Password
        onInputTextChanged: {
            console.log("PasswordTextInput onInputTextChanged");
        }
    }
    Rectangle{
        id: footerRec;
        width: passwordTextInput;
        anchors.left: parent.left;
        anchors.top: passwordTextInput.bottom;
        anchors.topMargin: 10;
        anchors.leftMargin: 75;
        CheckBox{
            id: checkRemember;
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.topMargin: 10;
            anchors.leftMargin: 10;
            checkState: 1;
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if (checkRemember.checkState == 1){
                        checkRemember.checkState = 2
                    }
                    else{
                       checkRemember.checkState = 1
                    }
                }
            }
        }
        Text{
            id: rememberText;
            anchors.top: parent.top;
            anchors.topMargin: 7;
            anchors.left: checkRemember.right;
            anchors.leftMargin: 7;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_small;
            text: qsTr("Remember me");
        }
        Text{
            id: forgotText;
            anchors.top: parent.top;
            anchors.topMargin: 7;
            anchors.left: rememberText.right;
            anchors.leftMargin: 120;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_small;
            text: '<html><style type="text/css"></style><a href="http://google.com">Forgot password?</a></html>'
            onLinkActivated: {
                Qt.openUrlExternally(link)
            }
        }
    }
    AuxButton{
        id: loginButton;
        anchors.top: footerRec.bottom;
        anchors.left: parent.left;
        anchors.topMargin: 50;
        anchors.leftMargin: (parent.width - width)/2;
        width: 100;
        height: 30;
        hasText: true;
        textButton: qsTr("Login");
        color: Style.buttonColor;
        onClicked: {
            console.log("LoginDialog loginButton onClicked");
            loginContainer.loaderDialog.closeItem();
        }
    }
}
