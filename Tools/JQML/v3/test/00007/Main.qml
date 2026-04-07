import QtQuick

Item {
    property int a: 5
    property int b: a * 2
    
    Component.onCompleted: {
        console.log("a =", a)
        console.log("b =", b)
        a = 10
        console.log("После изменения a:")
        console.log("a =", a)
        console.log("b =", b)
    }
}