import QtQuick

// Binding through an alias on the same object must replace the target's default binding.
Item {
    id: host
    width: 40
    height: 40

    property alias chosen: loader.sourceComponent
    chosen: overrideComp

    Loader {
        id: loader
        width: 20
        height: 20
        sourceComponent: defaultComp
    }

    Component {
        id: defaultComp
        Rectangle {
            objectName: "default"
            width: 10
            height: 10
        }
    }

    Component {
        id: overrideComp
        Rectangle {
            objectName: "override"
            width: 10
            height: 10
        }
    }

    Component.onCompleted: {
        console.log("name", loader.item.objectName)
        Qt.quit()
    }
}
