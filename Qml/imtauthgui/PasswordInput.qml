import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Column {
    id: root;
    spacing: Style.size_largeMargin;
    property alias oldPassword: currentPasswordInput.text;
    property alias newPassword: newPasswordInput1.text;
    property bool accepted: false;

    Component.onCompleted: {
        if (AuthorizationController.loggedUserIsSuperuser()){
            currentPasswordInput.visible = false;
            currentPasswordInput.text = " ";
        }
    }

    onFocusChanged: {
        if (root.focus){
            currentPasswordInput.focus = root.focus;
        }
    }

    function checkPassword(){
        root.accepted = newPasswordInput1.text !== "" && newPasswordInput1.text == newPasswordInput2.text;

        newPasswordInput1.acceptableInput = root.accepted;
        newPasswordInput2.acceptableInput = root.accepted;

        newPasswordInput2.bottomComp = root.accepted ? emptyComp : errorComp;
    }

    Component {
        id: errorComp;

        Text {
            text: qsTr("Passwords don't match");
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }
    }

    Component {
        id: emptyComp;

        BaseText {
        }
    }

    TextInputElementView {
        id: currentPasswordInput;
        width: parent.width;
        name: qsTr("Current password");
        echoMode: TextInput.Password;
        placeHolderText: qsTr("Enter the current password");
        onEditingFinished: {
        }

        KeyNavigation.tab: newPasswordInput1;
        KeyNavigation.backtab: newPasswordInput2;
    }

    GroupElementView {
        id: oldPasswordGroup;
        width: parent.width;
        TextInputElementView {
            id: newPasswordInput1;
            name: qsTr("New password");
            echoMode: TextInput.Password;
            placeHolderText: qsTr("Enter the new password");
            readOnly: currentPasswordInput.text === "";
            onTextChanged: {
                root.checkPassword();
            }
            KeyNavigation.tab: newPasswordInput2;
            KeyNavigation.backtab: currentPasswordInput;
        }

        TextInputElementView {
            id: newPasswordInput2;
            name: qsTr("Confirm password");
            echoMode: TextInput.Password;
            placeHolderText: qsTr("Confirm password");
            readOnly: currentPasswordInput.text === "";
            bottomComp: emptyComp;
            onTextChanged: {
                root.checkPassword();
            }
            KeyNavigation.tab: currentPasswordInput;
            KeyNavigation.backtab: newPasswordInput1;
        }
    }

    // BaseText {
    //     id: titlePassword;

    //     text: qsTr("Password");
    // }

    // CustomTextField {
    //     id: passwordTextInput;

    //     width: root.width;
    //     height: 30;

    //     placeHolderText: qsTr("Enter the password");
    //     echoMode: TextInput.Password;
    //     KeyNavigation.tab: confirmPasswordInput;

    //     readOnly: root.readOnly;
    // }

    // BaseText {
    //     id: titleConfirmPassword;

    //     text: qsTr("Confirm Password");
    // }

    // CustomTextField {
    //     id: confirmPasswordInput;

    //     width: root.width;
    //     height: 30;

    //     placeHolderText: qsTr("Confirm password");
    //     echoMode: TextInput.Password;

    //     readOnly: root.readOnly;
    // }

    // Item {
    //     width: root.width;
    //     height: 30;

    //     BaseText {
    //         id: textError;

    //         anchors.left: parent.left;
    //         anchors.verticalCenter: parent.verticalCenter;

    //         color: Style.errorTextColor;

    //         visible: false;

    //         text: qsTr("Passwords don't match");
    //     }
    // }
}


