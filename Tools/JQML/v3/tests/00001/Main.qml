import QtQuick

Item {
    property int a: 10
    property int b: a + 5

    Component.onCompleted: {
        console.log("a =", a)
        console.log("b =", b)
        console.log("Test: b == a + 5?", b === a + 5)
        Qt.quit();
    }
}