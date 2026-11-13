import QtQuick
import ModA 1.0 as M
import ModB 1.0 as M

Item {
    M.Thing { id: thing }
    M.OnlyA { id: onlyA }
    M.OnlyB { id: onlyB }

    Component.onCompleted: {
        console.log("thing", thing.name)
        console.log("style", M.Style.name)
        console.log("only", onlyA.name, onlyB.name)
        Qt.quit()
    }
}
