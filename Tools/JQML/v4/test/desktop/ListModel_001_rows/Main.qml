import QtQuick

Item {
    ListModel {
        id: records
        ListElement { name: "a" }
        ListElement { name: "b" }
    }

    Component.onCompleted: {
        console.log("count", records.count)
        console.log("row", records.get(0).name, records.get(1).name)
        Qt.quit()
    }
}
