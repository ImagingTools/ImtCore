import QtQuick

Item {
    ListModel {
        id: m
        ListElement { t: "x" }
        ListElement { t: "y" }
        ListElement { t: "z" }
    }

    Row {
        id: row
        spacing: 3
        Repeater {
            id: r
            model: m
            delegate: Rectangle { width: 10 + index; height: 8 }
        }
    }

    Component.onCompleted: {
        console.log("wd045", "row-repeater", m.count, r.count, row.spacing)
        Qt.quit()
    }
}
