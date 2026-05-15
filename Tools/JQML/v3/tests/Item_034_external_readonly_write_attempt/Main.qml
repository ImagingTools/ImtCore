import QtQuick

Item {
    ReadonlyChild {
        id: child
    }

    Component.onCompleted: {
        var writeFailed = false

        console.log("fixed", child.fixed)

        try {
            child.fixed = 9
        } catch (e) {
            writeFailed = true
        }

        console.log("readonly write", writeFailed, child.fixed)
        Qt.quit()
    }
}
