import QtQuick
import QtQml.Models

Item {
    id: root
    property int countChangedCalls: 0

    ListModel {
        id: model

        onCountChanged: {
            root.countChangedCalls = root.countChangedCalls + 1
            console.log("countChanged", count, root.countChangedCalls)
        }
    }

    Component.onCompleted: {
        model.append({ name: "A" })
        model.append({ name: "B" })
        model.insert(1, { name: "X" })
        model.remove(0, 1)
        model.clear()

        console.log("final", model.count, root.countChangedCalls)
        Qt.quit()
    }
}
