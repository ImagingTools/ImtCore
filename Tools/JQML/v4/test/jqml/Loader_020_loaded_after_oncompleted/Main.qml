import QtQuick

Item {
    id: root
    width: 200
    height: 200

    property var log: []

    Component {
        id: editorComp
        Item {
            property string paramId: ""
            property string name: ""
        }
    }

    ListModel {
        id: rows
        ListElement { key: "a" }
        ListElement { key: "b" }
    }

    Repeater {
        model: rows
        delegate: Column {
            id: paramItem
            property string rowKey: model.key

            Component.onCompleted: {
                console.log("completed", paramItem.rowKey)
                elementLoader.sourceComponent = editorComp
            }

            function itemOnLoaded(item) {
                item.paramId = rowKey
                console.log("itemOnLoaded", rowKey, item.paramId)
            }

            Loader {
                id: elementLoader
                onLoaded: {
                    console.log("onLoaded", paramItem.rowKey)
                    paramItem.itemOnLoaded(item)
                }
            }
        }
    }

    Timer {
        interval: 100
        running: true
        onTriggered: Qt.quit()
    }
}
