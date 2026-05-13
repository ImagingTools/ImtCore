import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: model
    }

    Component.onCompleted: {
        model.append([{ name: "A" }, { name: "B" }, { name: "C" }])
        model.insert(1, { name: "X" })

        console.log("afterInsert", model.count, model.get(0).name, model.get(1).name, model.get(2).name, model.get(3).name)

        model.remove(2, 1)
        console.log("afterRemove", model.count, model.get(0).name, model.get(1).name, model.get(2).name)

        model.clear()
        console.log("afterClear", model.count)
        Qt.quit()
    }
}
