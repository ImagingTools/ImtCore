import QtQuick
Item {
    id: root
    property rect area: Qt.rect(1, 2, 3, 4)
    property alias exposed: root.area
    property alias coordinate: root.exposed.x
    property bool observe: false
    onCoordinateChanged: { if(observe) console.log("notify", coordinate) }
    Component.onCompleted: {
        observe = true
        coordinate = 5
        console.log("write", area.x, area.y)
        area = Qt.rect(9, 8, 7, 6)
        console.log("updated", coordinate, exposed.y)
        Qt.quit()
    }
}
