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
                property string json: ""
            }
        }
        Component.onCompleted: editors["T"] = editorComp
    }

    QtObject { id: first; property string m_id: "Language"; property string m_typeId: "T"; property string m_name: "Lang"; property string m_data: "{1}" }
    QtObject { id: second; property string m_id: "Design"; property string m_typeId: "T"; property string m_name: "Look"; property string m_data: "{2}" }

    // the rows exist later: the parameters are created from JSON after the item that shows them
    ListModel {
        id: rows
        dynamicRoles: true
    }

    Loader {
        id: outer
        sourceComponent: Component {
            Column {
                id: host
                property var shown: holder.ready ? rows : 0

                Repeater {
                    model: host.shown
                    delegate: Column {
                        id: paramItem
                        property var parameter: model.item
                        property string paramId: parameter.m_id
                        property string paramTypeId: parameter.m_typeId
                        property string paramName: parameter.m_name
                        property string parameterJson: parameter.m_data

                        Component.onCompleted: {
                            if (paramTypeId in controller.editors) {
                                elementLoader.sourceComponent = controller.editors[paramTypeId]
                            }
                        }

                        function itemOnLoaded(item) {
                            if (item.paramId != undefined) {
                                item.paramId = paramId
                            }
                            if (item.name != undefined) {
                                item.name = paramName
                            }
                            item.json = parameterJson
                            console.log("itemOnLoaded", item.paramId, item.name, item.json)
                        }

                        Loader {
                            id: elementLoader
                            onLoaded: paramItem.itemOnLoaded(item)
                        }
                    }
                }
            }
        }
    }

    QtObject {
        id: holder
        property bool ready: false
    }

    Timer {
        interval: 20
        running: true
        onTriggered: {
            rows.append({ item: first })
            rows.append({ item: second })
            holder.ready = true
            done.start()
        }
    }

    Timer {
        id: done
        interval: 100
        onTriggered: Qt.quit()
    }
}
