import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 111 })
        m.append({ value: 111 + 1 })
        m.remove(0)
        console.log("wd111", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
