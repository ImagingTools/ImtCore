import QtQuick

Item {
    readonly property int value: 100

    readonly property var obj: Item {}

    Component.onCompleted: {
        console.log("value", value)
        console.log("typeof obj", typeof obj)
        try {
            value = 200
            console.log("readonly write", false)
        } catch (e) {
            console.log("readonly write", true)
        }

        try {
            obj = {}
            console.log("readonly write", false)
        } catch (e) {
            console.log("readonly write", true)
        }
        Qt.quit()
    }
}
