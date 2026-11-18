import QtQuick

// `filter.fieldId: ...` at the use site sets the property of the object that is the default of `filter` (the object of the file of Base),
// whatever the order the properties are written in; the copy of that object has the values.
Item {
    id: root

    Base {
        id: plain
    }

    Base {
        id: changed
        filter.fieldId: "Status"
        filter.valueType: "Integer"
    }

    Mid {
        id: mid
        filter.fieldId: "Own"
    }

    Component.onCompleted: {
        console.log("plain", JSON.stringify(plain.filter.copy()))
        console.log("changed", JSON.stringify(changed.filter.copy()))
        console.log("mid", JSON.stringify(mid.filter.copy()))
        console.log("separate", plain.filter !== changed.filter)
        Qt.quit()
    }
}
