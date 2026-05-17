import QtQuick

Item {
    Flow {
        id: flow
        spacing: 3
        padding: 2

        Item { width: 20; height: 10 }
        Item { width: 35; height: 15 }
        Item { width: 10; height: 5 }
    }

    Component.onCompleted: {
        console.log("flow", "created")
        Qt.quit()
    }
}
