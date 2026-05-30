import QtQuick

Item {
    ListModel { id: m }

    Loader {
        id: l
        sourceComponent: ListView {
            id: inner
            width: 100
            height: 40
            model: m
            delegate: Rectangle { width: 12; height: 8 }
        }
    }

    Component.onCompleted: {
        m.append({ value: 92 })
        m.append({ value: 92 + 1 })
        console.log("wd092", "loader-listview", m.count, l.width, l.height)
        Qt.quit()
    }
}
