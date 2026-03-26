import QtQuick

Item {
    function init() {
        console.log("init() вызвана")
    }

    Component.onCompleted: {
        init()
    }
}