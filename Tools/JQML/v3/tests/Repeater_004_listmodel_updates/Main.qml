import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: lm
    }

    Repeater {
        id: rep
        model: lm
        delegate: Item {
            property string nameValue: model.name
        }
    }

    Component.onCompleted: {
        lm.append([{ name: "A" }, { name: "B" }, { name: "C" }])
        console.log("step1", rep.count, rep.itemAt(0).nameValue, rep.itemAt(1).nameValue, rep.itemAt(2).nameValue)

        lm.insert(1, { name: "X" })
        lm.remove(2, 1)

        console.log("step2", rep.count, rep.itemAt(0).nameValue, rep.itemAt(1).nameValue, rep.itemAt(2).nameValue)
        Qt.quit()
    }
}
