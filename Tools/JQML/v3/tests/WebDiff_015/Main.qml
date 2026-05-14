import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 15 })
        m.append({ value: 15 + 1 })
        m.remove(0)
        console.log("wd015", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
