import QtQuick

Item {
    id: root

    signal ready()

    Loader {
        id: loader
        active: true
        source: "LoadedText.qml"

        onLoaded: root.ready()
    }

    function loaderState() {
        return loader.active ? "active" : "inactive"
    }
}