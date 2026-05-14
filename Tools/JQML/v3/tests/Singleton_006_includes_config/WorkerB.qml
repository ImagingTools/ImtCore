import QtQuick
import ModuleInc 1.0

Item {
    function step(tag, delta) {
        IncState.add(tag, delta)
        console.log("singleton6-b", tag, IncState.value, IncState.trace)
    }
}
