import QtQuick

Item {
    id: root
    property int a: 10
    property int b: a + 5

    Component.onCompleted: {
        console.log("a", a)
        console.log("b", b)
        console.log("sum", b === a + 5)
        Qt.quit()
    }
}
