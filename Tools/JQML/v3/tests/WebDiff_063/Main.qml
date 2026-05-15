import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 63 })
        m.append({ value: 63 + 1 })
        m.remove(0)
        console.log("wd063", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
