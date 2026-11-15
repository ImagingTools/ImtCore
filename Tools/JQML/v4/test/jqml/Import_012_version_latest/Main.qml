import QtQuick
import Mod

Item {
    Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        Qt.quit()
    }
}
