import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 87 })
        m.append({ value: 87 + 1 })
        m.remove(0)
        console.log("wd087", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
