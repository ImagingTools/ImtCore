import QtQuick 2.15
Item {
    id: root
    width: 300; height: 400
    property int step: 0
    property bool hl: false
    Component { id: hlc; Rectangle { color: "red" } }
    ListView {
        id: list
        anchors.fill: parent
        model: 200
        highlight: root.hl ? hlc : null
        delegate: Item { width: 300; height: 30 }
    }
    Timer {
        interval: 50; repeat: true; running: true
        onTriggered: {
            root.step++
            if (root.step === 1) { list.currentIndex = 150; console.log("no highlight: right after", list.contentY) }
            else if (root.step === 2) { console.log("no highlight: next tick", list.contentY); list.contentY = 0; root.hl = true }
            else if (root.step === 3) { list.currentIndex = 120; console.log("highlight: right after", list.contentY) }
            else if (root.step === 4) { console.log("highlight: next tick", list.contentY); list.forceActiveFocus(); list.contentY = 0; list.currentIndex = 0 }
            else if (root.step === 5) { list.incrementCurrentIndex(); for (var i = 0; i < 30; i++) list.incrementCurrentIndex(); console.log("increment x31 with highlight", list.contentY, list.currentIndex) }
            else { Qt.quit() }
        }
    }
}
