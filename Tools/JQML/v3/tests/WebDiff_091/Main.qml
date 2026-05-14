import QtQuick

Item {
    ListModel {
        id: m
        ListElement { v: 1 }
        ListElement { v: 2 }
    }

    ListView {
        id: v
        width: 100
        height: 40
        model: m
        currentIndex: 1
        delegate: Rectangle { width: 20; height: 10 }
    }

    Component.onCompleted: {
        console.log("wd091", "listview-current", m.count, v.currentIndex, v.count)
        Qt.quit()
    }
}
