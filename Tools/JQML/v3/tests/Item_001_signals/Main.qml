import QtQuick

Item {
    onWidthChanged: console.log("widthChanged", width)
    onHeightChanged: console.log("heightChanged", height)
    onXChanged: console.log("xChanged", x)
    onYChanged: console.log("yChanged", y)

    Component.onCompleted: {
        console.log("completed")
        width = 120
        height = 45
        x = 7
        y = 9
        Qt.quit()
    }
}
