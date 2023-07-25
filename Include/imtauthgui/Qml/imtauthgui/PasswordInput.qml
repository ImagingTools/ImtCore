import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Column {
    id: root;

    spacing: 10;

    width: 300;

    property alias password: passwordTextInput.text;
    property bool accepted: passwordTextInput.text !== "" && passwordTextInput.text === confirmPasswordInput.text;

//    Item {
//        width: root.width;
//        height: 30;

//        BaseText {
//            id: title;

//            font.family: Style.fontFamilyBold;

//            text: qsTr("Please enter the password for system administrator");
//        }
//    }

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
        KeyNavigation.tab: passwordTextInput;

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

            visible: !root.accepted;

            text: qsTr("Passwords don't match");
        }
    }
}


