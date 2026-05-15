import QtQuick
import "."

Item {
    UserA { id: a }
    UserB { id: b }

    Component.onCompleted: {
        SessionState.reset()
        console.log("singleton4-start", SessionState.stepCount, SessionState.lastTag === "" ? "-" : SessionState.lastTag)

        a.bump("A")
        b.bump("B")
        a.bump("A2")

        console.log("singleton4-final", SessionState.stepCount, SessionState.lastTag, SessionState.trace)
        Qt.quit()
    }
}
