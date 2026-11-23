import QtQuick

// SortFilterProxyModel: ValueFilter, FunctionFilter (its parameters are roles, invalidate()), inverted / enabled filters,
// RoleSorter and StringSorter with sortOrder and priority, FunctionSorter, a change of the source (dynamic), a Repeater
// and a ListView on the proxy
Item {
    width: 300; height: 300
    ListModel {
        id: people
        ListElement { name: "carol"; age: 35; team: "b" }
        ListElement { name: "alice"; age: 30; team: "a" }
        ListElement { name: "Bob"; age: 25; team: "a" }
        ListElement { name: "dave"; age: 30; team: "b" }
    }
    SortFilterProxyModel {
        id: byAge
        model: people
        sorters: [ RoleSorter { roleName: "age"; priority: 0 }, StringSorter { roleName: "name"; priority: 1 } ]
    }
    SortFilterProxyModel {
        id: teamA
        model: people
        filters: [ ValueFilter { id: vf; roleName: "team"; value: "a" } ]
        sorters: [ RoleSorter { roleName: "name"; sortOrder: Qt.DescendingOrder } ]
    }
    SortFilterProxyModel {
        id: fn
        model: people
        filters: [ FunctionFilter { id: ff; property int limit: 30; function filter(age: int): bool { return age <= limit } } ]
        sorters: [ FunctionSorter { function compare(lhs: var, rhs: var): int { return lhs.name.length - rhs.name.length } } ]
    }
    Column { Repeater { id: rep; model: byAge; Text { text: name + ":" + age } } }
    ListView { id: lv; x: 150; width: 100; height: 200; model: fn; delegate: Text { text: name } }
    function names(r) { let s = []; for (let i = 0; i < r.count; i++) s.push(r.itemAt ? r.itemAt(i).text : r.itemAtIndex(i).text); return s.join(",") }
    Component.onCompleted: {
        console.log("byAge", names(rep))
        console.log("teamA", teamA.rowCount(), teamA.columnCount(), teamA.index(1, 0).row)
        console.log("fn", lv.count)
        vf.inverted = true; console.log("inverted", teamA.rowCount())
        vf.enabled = false; console.log("disabled", teamA.rowCount())
        ff.limit = 25; ff.invalidate(); console.log("limit 25", lv.count)
        people.append({ name: "eve", age: 20, team: "a" }); console.log("appended", names(rep), lv.count)
        people.setProperty(0, "age", 10); console.log("changed", names(rep))
        Qt.quit()
    }
}
