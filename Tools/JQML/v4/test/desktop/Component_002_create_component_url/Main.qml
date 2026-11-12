import QtQuick

Item {
    id: root

    property var missing: null

    Component.onCompleted: {
        var component = Qt.createComponent("Box.qml")
        console.log("status", component.status, component.status === Component.Ready, component.errorString() === "")
        var box = component.createObject(root, { value: 7 })
        console.log("box", box.mark, box.width, box.value, box.parent === root)

        var second = component.createObject(null)
        console.log("second", second.mark, second.parent === null)

        // A file that does not exist: desktop Qt knows at once, the web engine after trying to fetch it.
        missing = Qt.createComponent("Missing.qml")
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("missing", missing.status === Component.Error, missing.errorString() !== "", missing.createObject(root) === null)
            Qt.quit()
        }
    }
}
