import QtQuick

Rectangle {
    id: rect
    width: 150
    height: 80

    onWidthChanged: console.log("widthChanged", width)
    onHeightChanged: console.log("heightChanged", height)

    Component.onCompleted: {
        width = 250
        height = 120
        console.log("final", width, height)
        Qt.quit()
    }
}
