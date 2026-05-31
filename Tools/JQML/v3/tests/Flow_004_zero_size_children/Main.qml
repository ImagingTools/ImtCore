import QtQuick

Item {
    Flow {
        id: flow
        width: 200
        spacing: 5

        Item { width: 50; height: 30 }
        Item { width: 0; height: 0 }
        Item { width: 70; height: 30 }
    }

    Component.onCompleted: {
        // Flow should include zero-size children in layout
        console.log("flow", "created")
        Qt.quit()
    }
}
