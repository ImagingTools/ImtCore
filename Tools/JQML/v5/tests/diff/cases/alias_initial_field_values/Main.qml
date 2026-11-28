import QtQuick

Item {
    id: root
    property int source: 9
    Component {
        id: template
        Box { fieldY: root.source }
    }
    Component.onCompleted: {
        let first = template.createObject(root, { fieldX: 15 })
        let second = template.createObject(root, { fieldX: 21, fieldY: 31 })
        console.log("initial", first.fieldX, first.fieldY, second.fieldX, second.fieldY)
        root.source = 10
        console.log("after source", first.fieldX, first.fieldY, second.fieldX, second.fieldY)
        first.destroy()
        Qt.quit()
    }
}
