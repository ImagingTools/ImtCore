import QtQuick

// Qt.md5() (the change-password dialogs of IotPlatform hash the password with it) and Qt.Key_Asterisk (BasicTreeView expands every branch on it).
Item {
    width: 40
    height: 40

    Component.onCompleted: {
        console.log("md5 type", typeof Qt.md5)
        console.log("md5 ab", Qt.md5("ab"))
        console.log("md5 empty", Qt.md5(""))
        console.log("md5 number", Qt.md5(12))
        console.log("md5 cyrillic", Qt.md5("пароль"))
        console.log("md5 same", Qt.md5("x") === Qt.md5("x"), Qt.md5("x") === Qt.md5("y"))
        console.log("Key_Asterisk", Qt.Key_Asterisk, Qt.Key_Plus, Qt.Key_Minus, Qt.Key_Slash)
        Qt.quit()
    }
}
