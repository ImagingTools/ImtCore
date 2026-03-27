import QtQuick

Item {
    Timer {
        interval: 100; running: true; repeat: false
        onTriggered: console.log("Таймер сработал")
    }

    Component.onCompleted: {
        console.log("Компонент создан")
    }
}