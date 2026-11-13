import QtQuick

Item {
    id: root

    property var source: null
    property var boundTitle: source?.name
    property int boxWidth: 40

    Component.onCompleted: {
        var show = function(value) {
            if (value === undefined)
                return "undefined"
            if (value === null)
                return "null"
            return "" + value
        }

        console.log("bound " + show(boundTitle))
        console.log("id " + show(root?.boxWidth))

        var obj = null
        console.log("prop-null " + show(obj?.name))
        obj = undefined
        console.log("prop-undef " + show(obj?.name))

        var person = {}
        person.name = "Ada"
        person.count = 0
        person.label = ""
        console.log("prop " + show(person?.name))
        console.log("zero " + show(person?.count))
        console.log("empty " + show(person?.label))

        var parent = {}
        parent.child = null
        console.log("nested-null " + show(parent?.child?.name))
        var child = {}
        child.name = "Bea"
        parent.child = child
        console.log("nested " + show(parent?.child?.name))

        var missing = null
        console.log("ternary " + (missing == null ? "none" : missing?.name))
        missing = person
        console.log("ternary-value " + (missing == null ? "none" : missing?.name))

        Qt.quit()
    }
}
