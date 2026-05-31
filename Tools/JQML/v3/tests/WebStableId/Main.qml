import QtQuick

// Exercises the stable DOM id generation: a static reused component (firstBox /
// secondBox) and a Repeater that instantiates the same component as delegates.
Item {
    id: root

    width: 200
    height: 200

    Rectangle {
        id: header

        width: 200
        height: 20

        Text {
            text: "title"
        }
    }

    Box {
        id: firstBox
        label: "A"
    }

    Box {
        id: secondBox
        label: "B"
    }

    Repeater {
        id: rep
        model: 3
        delegate: Box {
            label: "item"
        }
    }

    Component.onCompleted: {
        console.log("ready")
        Qt.quit()
    }
}
