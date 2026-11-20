import QtQuick 2.15
Item {
    id: root
    signal ping()
    function late() { console.log("late") }
    function second() { console.log("second") }
    function first() {
        console.log("first")
        root.ping.connect(root.late)
        root.ping.disconnect(root.second)
    }
    Component.onCompleted: {
        root.ping.connect(root.first)
        root.ping.connect(root.second)
        console.log("emit 1")
        root.ping()
        console.log("emit 2")
        root.ping()
        Qt.quit()
    }
}
