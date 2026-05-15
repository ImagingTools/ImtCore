import QtQuick

Item {
    id: root
    width: 100
    height: 100

    Item {
        id: item1
        focus: true
        onFocusChanged: {
            console.log('Item1', focus)
        }
    }

    Item {
        id: item2
        focus: true
        onFocusChanged: {
            console.log('Item2', focus)
        }
    }

    Item {
        id: item3
        focus: true
        onFocusChanged: {
            console.log('Item3', focus)
        }
    }

    Component.onCompleted: {
        Qt.quit()
    }
}
