import QtQuick

// the attached EnterKey (Qt 6.4): the enter key an on-screen keyboard shows (TextFieldThemed of QmlAppTemplate)
Item {
    TextInput { id: a; EnterKey.type: Qt.EnterKeyDone }
    TextEdit { id: b }
    Component.onCompleted: {
        console.log(a.EnterKey.type, b.EnterKey.type, Qt.EnterKeyDefault, Qt.EnterKeyReturn, Qt.EnterKeyGo, Qt.EnterKeySend, Qt.EnterKeySearch, Qt.EnterKeyNext, Qt.EnterKeyPrevious)
        b.EnterKey.type = Qt.EnterKeySearch
        console.log(b.EnterKey.type)
        Qt.quit()
    }
}
