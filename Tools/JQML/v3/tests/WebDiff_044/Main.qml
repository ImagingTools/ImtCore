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
        m.append({ value: 44 })
        m.append({ value: 44 + 1 })
        console.log("wd044", "loader-listview", m.count, l.width, l.height)
        Qt.quit()
    }
}
