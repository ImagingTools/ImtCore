import QtQuick
import Mod 2.0

Item {
    Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        Qt.quit()
    }
}
