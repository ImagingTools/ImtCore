import QtQuick

Item {
    ListView {
        id: view
        width: 100
        height: 100
        spacing: 5
        model: 3
        delegate: Item {
            width: 50
            height: 20
        }
    }

    Component.onCompleted: {
        console.log("before", view.count)
        view.orientation = ListView.Horizontal
        console.log("after", view.count)
        Qt.quit()
    }
}
