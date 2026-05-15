import QtQuick
import "."

Item {
    function bump(tag) {
        SessionState.step(tag)
        console.log("singleton4-b", tag, SessionState.stepCount, SessionState.lastTag)
    }
}
