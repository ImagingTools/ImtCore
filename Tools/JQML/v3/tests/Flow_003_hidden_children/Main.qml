import QtQuick

Item {
    Flow {
        id: flow
        width: 200
        spacing: 5

        Item { width: 50; height: 30 }
        Item { width: 60; height: 30; visible: false }
        Item { width: 70; height: 30 }
    }

    Component.onCompleted: {
        // Flow should exclude invisible children from layout
        console.log("flow", "created")
        Qt.quit()
    }
}
