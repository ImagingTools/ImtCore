import QtQuick

// A Binding writes its value when the value changes. A script that writes the target afterwards replaces the binding of the target
// and wins, until the value of the Binding changes again (the DialogDecorator of the Agentino dialogs: the decorator takes the height of
// the control through a Binding, and its column sets the height of the decorator by a script). While the target keeps its own
// binding the Binding keeps winning over it.
Item {
    id: root

    property real pushed: 100
    property real source: 40

    // the target has a binding of its own (height of the column), like the decorator
    Item { id: scripted; height: root.source }
    Binding { id: bindScripted; target: scripted; property: "height"; value: root.pushed }

    // the target has a binding of its own and nothing writes it by a script
    Item { id: bound; height: root.source }
    Binding { id: bindBound; target: bound; property: "height"; value: root.pushed }

    property int step: 0

    Timer {
        interval: 1
        running: true
        repeat: true
        onTriggered: {
            root.step++
            if (root.step === 1) {
                console.log("start", scripted.height, bound.height)
            } else if (root.step === 2) {
                // the source of the own binding changes: the Binding still wins
                root.source = 55
            } else if (root.step === 3) {
                console.log("source changed", scripted.height, bound.height)
                // a script writes one target
                scripted.height = 77
            } else if (root.step === 4) {
                console.log("script write", scripted.height, bound.height)
                root.source = 66
            } else if (root.step === 5) {
                console.log("source changed again", scripted.height, bound.height)
                root.pushed = 120
            } else if (root.step === 6) {
                console.log("value changed", scripted.height, bound.height)
                Qt.quit()
            }
        }
    }
}
