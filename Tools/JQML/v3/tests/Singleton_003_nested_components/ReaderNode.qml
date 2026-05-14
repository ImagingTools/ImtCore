import QtQuick
import "."

Item {
    function touch(tag, delta) {
        SharedState.add(tag, delta)
        console.log("singleton3-touch", tag, SharedState.value, SharedState.history)
    }
}
