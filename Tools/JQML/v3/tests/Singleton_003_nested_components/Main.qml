import QtQuick
import "."

Item {
    ReaderNode { id: left }
    ReaderNode { id: right }

    Component.onCompleted: {
        SharedState.reset()
        console.log("singleton3-start", SharedState.value, SharedState.history === "" ? "-" : SharedState.history)

        left.touch("L", 2)
        right.touch("R", 3)
        left.touch("L2", 1)

        console.log("singleton3-final", SharedState.value, SharedState.history)
        Qt.quit()
    }
}
