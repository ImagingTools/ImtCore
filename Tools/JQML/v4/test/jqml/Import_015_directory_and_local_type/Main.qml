import QtQuick
import "sub"

Item {
    Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        Qt.quit()
    }
}
