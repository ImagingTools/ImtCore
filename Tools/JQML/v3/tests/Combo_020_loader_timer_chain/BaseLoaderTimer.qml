import QtQuick

Item {
    id: root

    property int baseState: 1

    signal baseReady()

    Loader {
        id: loader
        sourceComponent: Rectangle {
            width: 10
            height: 10
        }

        onLoaded: root.baseReady()
    }

    function baseLabel() {
        return "loader:" + baseState
    }
}