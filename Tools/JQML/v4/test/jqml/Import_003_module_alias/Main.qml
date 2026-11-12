import QtQuick
import Mod 1.0 as M

Item {
    M.Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        console.log("singleton", M.Counter.name, M.Counter.value)
        Qt.quit()
    }
}
