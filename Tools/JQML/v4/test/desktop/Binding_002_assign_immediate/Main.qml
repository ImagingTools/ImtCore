import QtQuick

// The new value of `a` is read on the next line. Qt updates `b` before that read.
Item {
    id: root
    property int a: 10
    property int b: a + 5

    Component.onCompleted: {
        a = 20
        console.log("b", b)
        Qt.quit()
    }
}
