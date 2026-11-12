import QtQuick

Item {
    id: root

    // `flag && missing` is undefined while the object is absent. That write must not abort the document,
    // and the binding must keep following the object once it appears.
    property bool compl: false
    property var decorator
    property bool ready: compl && decorator

    // A binding that yields undefined for an int is refused, and the following logs still run.
    property int broken: decorator

    Component.onCompleted: {
        console.log("idle", ready, broken)
        compl = true
        console.log("missing", ready)
        decorator = root
        console.log("set", ready, broken)
        Qt.quit()
    }
}
