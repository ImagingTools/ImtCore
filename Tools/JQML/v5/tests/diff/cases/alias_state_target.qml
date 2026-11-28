import QtQuick

Item {
    id: root
    property int source: 7
    property alias exposed: internal.width

    Item { id: internal; width: root.source }
    states: State {
        name: "override"
        PropertyChanges { target: root; exposed: 80 }
    }

    Component.onCompleted: {
        console.log("initial", exposed)
        root.state = "override"
        root.source = 9
        console.log("active", exposed)
        root.state = ""
        console.log("restored", exposed)
        Qt.quit()
    }
}
