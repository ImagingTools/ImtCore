import QtQuick 2.12

// More than 30 Loaders are made before `later`: a pass of the bindings after each of them would count 30 failures of `w`
// (it reads an object that does not exist yet), and the binding would be dropped for good.
Item {
    id: root
    width: 300
    height: 300
    property int w: later.width
    property int h: later.height + 1

    Column {
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
        Loader { sourceComponent: Rectangle { width: 10; height: 10 } }
    }

    Rectangle {
        id: later
        width: 77
        height: 20
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("w", root.w, "h", root.h)
            later.width = 90
            console.log("w after change", root.w)
            Qt.quit()
        }
    }
}
