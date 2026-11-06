import QtQuick

Item {
    Loader {
        id: loader
        sourceComponent: Component {
            Rectangle {
                width: 12
                height: 8
            }
        }
    }

    Component.onCompleted: {
        console.log("item", loader.item ? loader.item.width : -1, loader.item ? loader.item.height : -1)
        // Qt prints the Loader.Status enum as a number (Ready === 1).
        console.log("status", loader.status)
        Qt.quit()
    }
}
