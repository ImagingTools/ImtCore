import QtQuick

Item {
    id: root
    property int source: 5
    property int pendingValue: 30
    property alias exposed: cell.width
    property Binding overrideBinding: null

    Item { id: cell; width: root.source }

    Component {
        id: temporary
        Binding {
            target: root
            property: "exposed"
            delayed: true
            value: root.pendingValue
        }
    }

    Component.onCompleted: {
        root.overrideBinding = temporary.createObject(root)
        root.source = 9
        console.log("during", exposed)
        root.pendingValue = 40
        root.overrideBinding.destroy()
        afterDestruction.start()
    }

    Timer {
        id: afterDestruction
        interval: 80
        onTriggered: {
            console.log("after destruction", exposed, root.overrideBinding === null)
            root.source = 11
            console.log("after source", exposed)
            Qt.quit()
        }
    }
}
