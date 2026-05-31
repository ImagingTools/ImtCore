import QtQuick

Item {
    ListModel { id: m }

    Component.onCompleted: {
        m.append({ value: 27 })
        m.append({ value: 27 + 1 })
        m.remove(0)
        console.log("wd027", "listmodel-mutate", m.count)
        Qt.quit()
    }
}
