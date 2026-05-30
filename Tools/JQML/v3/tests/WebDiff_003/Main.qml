import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 3 })
        m.append({ value: 3 + 1 })
        m.remove(0)
        console.log("wd003", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
