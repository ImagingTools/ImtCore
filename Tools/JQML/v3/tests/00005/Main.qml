pragma ComponentBehavior: Bound
import QtQuick

Item {
    Component.onCompleted: {
        console.log("ComponentBehavior: Bound")
        Qt.quit()
    }
}