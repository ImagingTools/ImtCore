import QtQuick

Item {
    width: 100
    height: 100

    Base {
        id: first
        foo: 5
        decorator: Component { Item { objectName: "made" } }
    }

    Base {
        id: second
        decorator: Component { Item { objectName: "made2" } }
    }

    Base {
        id: third
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("first foo", first.foo, "changes", first.fooChanges, "made", first.made !== null ? first.made.objectName : "null")
            console.log("second foo", second.foo, "changes", second.fooChanges, "made", second.made !== null ? second.made.objectName : "null")
            console.log("third foo", third.foo, "changes", third.fooChanges, "made", third.made !== null ? third.made.objectName : "null")
            Qt.quit()
        }
    }
}
