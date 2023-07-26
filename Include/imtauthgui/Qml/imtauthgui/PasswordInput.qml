import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Column {
    id: root;

    spacing: 7;

    width: 300;

    property alias password: passwordTextInput.text;
    property bool accepted: !readOnly && passwordTextInput.text != "" && passwordTextInput.text === confirmPasswordInput.text;

    property alias passwordInput: passwordTextInput;
    property alias confirmInput: confirmPasswordInput;
    property alias errorText: errorText;

    property bool readOnly: false;

    onAcceptedChanged: {
        errorText.visible = !accepted;
    }

    onPasswordChanged: {
        let isError = false;
        if (root.password === ""){
            errorText.text = qsTr("Password cannot be empty");
            isError = true;
        }
        else{
            if (passwordTextInput.text !== confirmPasswordInput.text){
                errorText.text = qsTr("Passwords don't match");
                isError = true;
            }
        }

        errorText.visible = isError;
    }

    onFocusChanged: {
        console.log("InputBody onFocusChanged", focus);

        if (root.focus){
            passwordTextInput.focus = root.focus;
        }
    }

    BaseText {
        id: titlePassword;

        text: qsTr("Password");
    }

    CustomTextField {
        id: passwordTextInput;

        width: root.width;
        height: 30;

        placeHolderText: qsTr("Enter the password");
        echoMode: TextInput.Password;
        KeyNavigation.tab: confirmPasswordInput;

        readOnly: root.readOnly;

        Loader{
            id: inputDecoratorLoader1;

            sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
            onLoaded: {}
        }
    }

    BaseText {
        id: titleConfirmPassword;

        text: qsTr("Confirm Password");
    }

    CustomTextField {
        id: confirmPasswordInput;

        width: root.width;
        height: 30;

        placeHolderText: qsTr("Confirm password");
        echoMode: TextInput.Password;

        readOnly: root.readOnly;

        Loader{
            id: inputDecoratorLoader2;

            sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
            onLoaded: {}
        }
    }

    Item {
        width: root.width;
        height: 30;

        BaseText {
            id: errorText;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;

            color: Style.errorTextColor;

            visible: false;

            text: qsTr("Passwords don't match");
        }
    }
}


