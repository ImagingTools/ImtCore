import QtQuick
import ModB 1.0
import ModA 1.0

Item {
    Thing { id: thing }

    Component.onCompleted: {
        console.log("thing", thing.name)
        console.log("style", Style.name)
        Qt.quit()
    }
}
