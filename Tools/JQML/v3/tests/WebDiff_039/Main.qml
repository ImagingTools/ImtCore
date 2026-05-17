import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 39 })
        m.append({ value: 39 + 1 })
        m.remove(0)
        console.log("wd039", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
