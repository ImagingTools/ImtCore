import QtQuick

Item {
    id: root
    objectName: "root"

    Component {
        id: innerComponent
        Item {
            objectName: "inner"
            property int mark: 0
            Component.onCompleted: {
                console.log("inner parent=" + (parent ? parent.objectName : "null") + " mark=" + mark)
            }
        }
    }

    Component {
        id: outerComponent
        Item {
            objectName: "outer"
            Component.onCompleted: {
                console.log("outer before")
                var inner = innerComponent.createObject(root, { mark: 7 })
                inner.mark = 8
                console.log("outer after mark=" + inner.mark + " name=" + inner.objectName)
            }
        }
    }

    Timer {
        interval: 1
        running: true
        repeat: false
        onTriggered: {
            var obj = innerComponent.createObject(root)
            obj.mark = 4
            console.log("timer mark=" + obj.mark + " parent=" + obj.parent.objectName)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        outerComponent.createObject(root)
    }
}
