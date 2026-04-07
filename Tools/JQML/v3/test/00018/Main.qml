import QtQuick

Item {
    Item {
        id: item
        property var counter: 0

        Component.onCompleted: {
            counter = 5
            console.log("counter =", counter)
        }
    }

    Connections {
        target: item
        function onCounterChanged() {
            console.log("Сигнал counterChanged сработал")
        }
    }
}