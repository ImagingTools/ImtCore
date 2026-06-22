import QtQuick

Item {
    Component {
        id: comp
        Item {
            property int token: 42
        }
    }

    Loader {
        id: loader
        active: false

        Component.onCompleted: {
            setSource("NeverUsed.qml", { token: 77 })
            sourceComponent = comp
            active = true
        }

        onLoaded: {
            console.log("loader015", item.token)
            Qt.quit()
        }
    }
}
