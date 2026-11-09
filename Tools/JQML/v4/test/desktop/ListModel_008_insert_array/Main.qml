import QtQuick

Item {
    ListModel {
        id: records
        ListElement { name: "a" }
        ListElement { name: "d" }
    }

    // insert() takes one row or an array of rows, as append() does; remove() takes a count
    // (PermissionsTableView of ImtCore opens and closes a branch of its tree with them).
    Component.onCompleted: {
        records.insert(1, [{ name: "b" }, { name: "c" }])
        console.log("array", records.count, records.get(0).name, records.get(1).name, records.get(2).name, records.get(3).name)
        records.insert(0, { name: "z" })
        console.log("one", records.count, records.get(0).name, records.get(1).name)
        records.remove(2, 2)
        console.log("remove", records.count, records.get(0).name, records.get(1).name, records.get(2).name)
        records.insert(2, [{ name: "b" }, { name: "c" }])
        console.log("again", records.count, records.get(2).name, records.get(3).name, records.get(4).name)
        Qt.quit()
    }
}
