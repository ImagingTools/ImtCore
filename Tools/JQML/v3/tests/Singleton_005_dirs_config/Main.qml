import QtQuick
import MyCfg 1.0

Item {
    Component.onCompleted: {
        ConfState.reset()
        console.log("singleton5-start", ConfState.count, ConfState.last)

        ConfState.bump("A", 2)
        ConfState.bump("B", 3)

        console.log("singleton5-final", ConfState.count, ConfState.last)
        Qt.quit()
    }
}
