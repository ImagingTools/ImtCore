import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 51 })
        m.append({ value: 51 + 1 })
        m.remove(0)
        console.log("wd051", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
