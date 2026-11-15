import QtQuick
import ModB 1.0

Item {
    UsesA { id: usesA }
    Thing { id: thing }

    Component.onCompleted: {
        console.log("main", thing.name, Style.name)
        console.log("uses A", usesA.thingName, usesA.styleName)
        Qt.quit()
    }
}
