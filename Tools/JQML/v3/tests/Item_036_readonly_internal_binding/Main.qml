import QtQuick

Item {
    property int base: 2
    readonly property int doubled: base * 2

    Component.onCompleted: {
        console.log("start", doubled)
        base = 3
        console.log("after", doubled)
        Qt.quit()
    }
}
