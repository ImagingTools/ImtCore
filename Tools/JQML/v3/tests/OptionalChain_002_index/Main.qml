import QtQuick

Item {
    property var source: null
    property string keyName: "name"
    property var viaKey: source?.[keyName]

    Component.onCompleted: {
        var show = function(value) {
            if (value === undefined)
                return "undefined"
            if (value === null)
                return "null"
            return "" + value
        }

        console.log("bound-key " + show(viaKey))

        var obj = null
        console.log("chain-null " + show(obj?.items[0].name))

        var empty = {}
        empty.items = []
        var thrown = "no"
        try {
            var ignored = empty?.items[0].name
            thrown = "value " + show(ignored)
        } catch (error) {
            thrown = "throw"
        }
        console.log("chain-empty " + thrown)

        var row = {}
        row.name = "city"
        var items = []
        items.push(row)
        var holder = {}
        holder.items = items
        console.log("chain " + show(holder?.items[0].name))

        var list = null
        console.log("index-null " + show(list?.[0]))
        list = []
        list.push("only")
        console.log("index " + show(list?.[0]))
        console.log("deep-miss " + show(list?.[1]?.name))

        var entry = null
        list = []
        list.push(entry)
        console.log("deep-null " + show(list?.[0]?.name))
        list = []
        list.push(row)
        console.log("deep " + show(list?.[0]?.name))

        var key = "title"
        var map = {}
        map.title = "Map"
        console.log("key " + show(map?.[key]))
        console.log("key-miss " + show(map?.["missing"]))

        Qt.quit()
    }
}
