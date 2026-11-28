import QtQuick

Item {
    id: root
    property alias primary: child.width
    property alias secondary: root.primary

    Item { id: child; width: 5 }
    NumberAnimation { id: animation; target: root; property: "secondary"; from: 5; to: 42; duration: 100 }
    NumberAnimation { id: unknownProperty; target: root; property: "hasOwnProperty"; from: 0; to: 1; duration: 100 }
    Binding { target: root; property: "isPrototypeOf"; value: 10 }

    Component.onCompleted: {
        animation.start()
        animation.complete()
        unknownProperty.start()
        unknownProperty.complete()
        console.log("complete", child.width, root.primary, root.secondary)
        Qt.quit()
    }
}
