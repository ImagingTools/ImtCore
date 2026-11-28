import QtQuick

Item {
    id: root
    property int source: 5

    Component.onCompleted: {
        let component = Qt.createQmlObject('import QtQuick; Item {
            id: generated
            property int input: 2
            property rect position: Qt.rect(1, 2, 3, 4)
            property alias coordinateX: generated.position.x
            property alias coordinateY: generated.position.y
            coordinateY: generated.input
        }', root, "DynamicBindings")
        component.coordinateX = Qt.binding(function(){ return root.source })
        console.log("initial", component.coordinateX, component.coordinateY)
        root.source = 7
        component.input = 4
        console.log("updated", component.coordinateX, component.coordinateY)
        component.coordinateX = 11
        root.source = 8
        component.input = 6
        console.log("detached", component.coordinateX, component.coordinateY)
        Qt.quit()
    }
}
