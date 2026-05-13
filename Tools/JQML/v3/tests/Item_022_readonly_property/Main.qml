import QtQuick

Item {
    readonly property int value: 100

    Component.onCompleted: {
        console.log("value", value)
        try {
            value = 200
            console.log("readonly write", false)
        } catch (e) {
            console.log("readonly write", true)
        }
        Qt.quit()
    }
}
