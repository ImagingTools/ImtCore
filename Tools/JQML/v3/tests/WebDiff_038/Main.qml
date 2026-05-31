import QtQuick

Item {
    ListModel {
        id: m
        ListElement { value: 1 }
        ListElement { value: 2 }
        ListElement { value: 3 }
    }

    ListView {
        id: v
        width: 120
        height: 50
        model: m
        delegate: Rectangle { width: 20; height: 10 }
    }

    Component.onCompleted: {
        console.log("wd038", "listview-basic", m.count, v.count)
        Qt.quit()
    }
}
