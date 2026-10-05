import QtQuick

Item {
    ListView {
        id: view
        width: 200
        height: 200
        model: 3
        delegate: Item {
            width: 80
            height: 20
        }
    }

    Component.onCompleted: {
        console.log("before", view.count, view.contentHeight)
        view.model = 0
        console.log("after", view.count, view.contentHeight)
        Qt.quit()
    }
}
