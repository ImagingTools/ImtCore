import QtQuick
import ModuleInc 1.0

Item {
    WorkerA { id: a }
    WorkerB { id: b }

    Component.onCompleted: {
        IncState.reset()
        console.log("singleton6-start", IncState.value, IncState.trace)

        a.step("A", 1)
        b.step("B", 4)

        console.log("singleton6-final", IncState.value, IncState.trace)
        Qt.quit()
    }
}
