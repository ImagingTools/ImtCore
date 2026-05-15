import QtQuick

Item {
    property bool ready: false
    property int baseX: 10
    property int baseY: 20
    property int side: 30

    x: baseX
    y: baseY
    width: side * 2
    height: width + 5

    onXChanged: if (ready) console.log("xChanged", x)
    onYChanged: if (ready) console.log("yChanged", y)
    onWidthChanged: if (ready) console.log("widthChanged", width)
    onHeightChanged: if (ready) console.log("heightChanged", height)

    Component.onCompleted: {
        console.log("initial", x, y, width, height)
        ready = true
        baseX = 15
        baseY = 25
        side = 40
        console.log("updated", x, y, width, height)
        Qt.quit()
    }
}
