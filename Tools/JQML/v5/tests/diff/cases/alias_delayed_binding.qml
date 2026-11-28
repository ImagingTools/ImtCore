import QtQuick

Item {
    id: root
    property int source: 1
    property int nextValue: 10
    property bool overrideValue: false
    property alias output: cell.width
    property int stage: 0

    Item { id: cell; width: root.source }

    Binding {
        target: root
        property: "output"
        delayed: true
        when: root.overrideValue
        value: root.nextValue
    }

    Timer {
        interval: 80
        repeat: true
        running: true
        onTriggered: {
            root.stage++
            if (root.stage === 1) {
                console.log("initial", output)
                root.overrideValue = true
                root.nextValue = 11
                root.nextValue = 12
            } else if (root.stage === 2) {
                console.log("applied", output)
                root.source = 4
                root.overrideValue = false
            } else {
                console.log("restored", output)
                Qt.quit()
            }
        }
    }
}
