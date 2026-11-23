import QtQuick
import QtQuick.Controls

// The indicator of the Basic Switch and its handle, off and on, in a Column (the gallery's Switch page)
Item {
    width: 300; height: 200
    Column {
        Switch { id: a; text: "First" }
        Switch { id: b; text: "Second"; checked: true }
    }
    Timer { running: true; interval: 400; onTriggered: {
        for (let s of [a, b]) {
            let h = s.indicator.children[0]
            console.log(s.indicator.x, s.indicator.y, s.indicator.width, s.indicator.height, h.x, h.y, h.width, h.height, h.radius, s.position, s.visualPosition)
        }
        Qt.quit()
    } }
}
