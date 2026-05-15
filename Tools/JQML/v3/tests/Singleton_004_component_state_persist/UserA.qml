import QtQuick
import "."

Item {
    function bump(tag) {
        SessionState.step(tag)
        console.log("singleton4-a", tag, SessionState.stepCount, SessionState.lastTag)
    }
}
