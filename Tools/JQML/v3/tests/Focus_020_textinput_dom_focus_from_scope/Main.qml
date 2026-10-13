import QtQuick

Window {
    id: root
    width: 360
    height: 160
    visible: true

    FocusScope {
        id: loginField
        x: 10
        y: 10
        width: 200
        height: 32

        TextInput {
            id: loginInput
            anchors.fill: parent
            text: "user"
            focus: true
        }
    }

    FocusScope {
        id: passwordField
        x: 10
        y: 50
        width: 200
        height: 32

        TextInput {
            id: passwordInput
            anchors.fill: parent
            text: "secret"
            echoMode: TextInput.Password
        }
    }

    Timer {
        id: stepTimer
        interval: 50
        repeat: false
        onTriggered: {
            loginField.forceActiveFocus()
            console.log("focus020-login", loginField.activeFocus, loginInput.activeFocus, passwordField.activeFocus, passwordInput.activeFocus)
            passwordInput.focus = true
            passwordField.forceActiveFocus()
            console.log("focus020-password", loginField.activeFocus, loginInput.activeFocus, passwordField.activeFocus, passwordInput.activeFocus)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        stepTimer.start()
    }
}
