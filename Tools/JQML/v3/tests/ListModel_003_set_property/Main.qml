import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: model
    }

    Component.onCompleted: {
        model.append({ title: "T", value: 10 })
        model.setProperty(0, "value", 20)
        console.log("setProperty", model.get(0).title, model.get(0).value)

        model.get(0).value = 30
        console.log("directSet", model.get(0).title, model.get(0).value)
        Qt.quit()
    }
}
