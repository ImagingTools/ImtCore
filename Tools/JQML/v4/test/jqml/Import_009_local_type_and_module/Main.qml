import QtQuick
import Mod 1.0

Item {
    Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        Qt.quit()
    }
}
