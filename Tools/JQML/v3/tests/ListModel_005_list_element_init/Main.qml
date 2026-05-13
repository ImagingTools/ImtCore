import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: model

        ListElement {
            name: "A"
            value: 1
        }
        ListElement {
            name: "B"
            value: 2
        }
    }

    Component.onCompleted: {
        console.log("initialRows", model.get(0).name, model.get(0).value, model.get(1).name, model.get(1).value)
        model.append({ name: "C", value: 3 })
        console.log("afterAppend", model.count, model.get(2).name, model.get(2).value)
        Qt.quit()
    }
}
