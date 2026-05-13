import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: model
    }

    Component.onCompleted: {
        model.append({ name: "A", value: 1 })
        model.append([{ name: "B", value: 2 }, { name: "C", value: 3 }])

        console.log("count", model.count)
        console.log("row0", model.get(0).name, model.get(0).value)
        console.log("row1", model.get(1).name, model.get(1).value)
        console.log("row2", model.get(2).name, model.get(2).value)
        Qt.quit()
    }
}
