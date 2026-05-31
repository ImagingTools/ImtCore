import QtQuick
import "."

Item {
    Component.onCompleted: {
        console.log("singleton1-start", AppState.counter, AppState.label)
        AppState.mark("A")
        console.log("singleton1-after-a", AppState.counter, AppState.label)

        AppState.mark("B")
        console.log("singleton1-after-b", AppState.counter, AppState.label)

        Qt.quit()
    }
}
