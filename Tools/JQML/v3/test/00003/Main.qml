import QtQuick

Item {
    Item {
        id: item
        signal mySignal(string msg)
        Component.onCompleted: {
            mySignal("Первый вызов")
            mySignal("Второй вызов")
        }
    }
    Connections {
        target: item
        function onMySignal(msg) {
            console.log("Получено:", msg)
        }
    }
}
