import QtQuick
import QtQuick.Window

// Window.activeFocusItem and activeFocusItemChanged, after the activeFocusChanged of the items
// (DesktopApplication of QmlAppTemplate: onActiveFocusItemChanged)
Window {
    id: w
    width: 200; height: 200
    visible: true
    property var seen: []
    onActiveFocusItemChanged: seen.push("window " + (activeFocusItem ? activeFocusItem.objectName : "null"))
    Rectangle { id: a; objectName: "a"; width: 50; height: 50; onActiveFocusChanged: w.seen.push("a " + activeFocus) }
    Rectangle { id: b; objectName: "b"; x: 60; width: 50; height: 50; onActiveFocusChanged: w.seen.push("b " + activeFocus) }
    Timer {
        running: true; interval: 100
        onTriggered: {
            w.seen = []
            a.forceActiveFocus()
            b.forceActiveFocus()
            console.log(w.seen.join(", "))
            console.log(w.activeFocusItem === b, w.activeFocusItem.objectName)
            Qt.quit()
        }
    }
}
