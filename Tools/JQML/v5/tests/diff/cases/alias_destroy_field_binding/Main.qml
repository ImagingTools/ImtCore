import QtQuick

Item {
    id: root
    property int horizontal: 5
    property int vertical: 6
    property bool keepValue: false
    property int stage: 0

    Box {
        id: box
        fieldX: root.horizontal
        fieldY: root.vertical
    }

    Component {
        id: overrideComponent
        Binding {
            target: box
            property: "fieldX"
            value: 20
            restoreMode: root.keepValue ? Binding.RestoreNone : Binding.RestoreBindingOrValue
        }
    }

    function applyOverride(){
        let temporary = overrideComponent.createObject(root)
        root.horizontal++
        root.vertical++
        console.log("override", box.fieldX, box.fieldY)
        temporary.destroy()
    }

    Component.onCompleted: root.applyOverride()
    Timer {
        running: true
        repeat: true
        interval: 80
        onTriggered: {
            if (root.stage === 0) {
                console.log("restored fields", box.fieldX, box.fieldY)
                root.stage = 1
                root.keepValue = true
                root.applyOverride()
            } else {
                root.horizontal++
                root.vertical++
                console.log("kept override", box.fieldX, box.fieldY)
                Qt.quit()
            }
        }
    }
}
