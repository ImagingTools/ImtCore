import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 75 })
        m.append({ value: 75 + 1 })
        m.remove(0)
        console.log("wd075", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
