import QtQuick
import Mod 1.0

Item {
    Thing { id: thing }
    OnlyA { id: only }

    Component.onCompleted: {
        console.log("thing", thing.name)
        console.log("style", Style.name)
        console.log("only", only.name)
        Qt.quit()
    }
}
