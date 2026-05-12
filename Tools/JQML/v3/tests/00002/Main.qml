import QtQuick

Item {
    property int value: 0
    onValueChanged: console.log("Signal valueChanged:", value)

    Component.onCompleted: {
        value = 10
        value = 20
        console.log("Test: last value =", value)
        Qt.quit()
    }
}