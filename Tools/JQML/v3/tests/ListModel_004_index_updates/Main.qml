import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: model
    }

    Component.onCompleted: {
        model.append([{ name: "A" }, { name: "B" }, { name: "C" }])
        console.log("rows1", model.get(0).name, model.get(1).name, model.get(2).name)

        model.insert(0, { name: "X" })
        console.log("rows2", model.get(0).name, model.get(1).name, model.get(2).name, model.get(3).name)

        model.remove(1, 2)
        console.log("rows3", model.get(0).name, model.get(1).name)
        Qt.quit()
    }
}
