import QtQuick
import "a"
import "b"

Item {
    Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        Qt.quit()
    }
}
