import QtQuick

Item {
    id: root
    width: 200
    height: 200

    QtObject {
        id: controller
        property var editors: ({})
        property Component editorComp: Component {
            Item {
                property string paramId: ""
                property string name: ""
            }
        }

        Component.onCompleted: {
            editors["T"] = editorComp
        }
    }

    ListModel {
        id: rows
        ListElement { key: "a"; typeId: "T" }
        ListElement { key: "b"; typeId: "T" }
    }

    Loader {
        id: outer
        sourceComponent: Component {
            Column {
                Repeater {
                    model: rows
                    delegate: Column {
                        id: paramItem
                        property string rowKey: model.key
                        property string rowType: model.typeId

                        Component.onCompleted: {
                            if (paramItem.rowType in controller.editors) {
                                elementLoader.sourceComponent = controller.editors[paramItem.rowType]
                            }
                            console.log("completed", paramItem.rowKey, elementLoader.item !== null)
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
            }
        }
    }

    Timer {
        interval: 100
        running: true
        onTriggered: Qt.quit()
    }
}
