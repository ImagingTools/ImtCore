import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 99 })
        m.append({ value: 99 + 1 })
        m.remove(0)
        console.log("wd099", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
