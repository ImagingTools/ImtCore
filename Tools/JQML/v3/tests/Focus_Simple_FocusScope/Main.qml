import QtQuick

Item {
    id: root
    width: 100
    height: 100

    FocusScope {
        id: fs1
        width: 100
        height: 33
        focus: true
        onFocusChanged: {
            console.log('FocusScope1', focus)
        }

        Item {
            focus: true
            onFocusChanged: {
                console.log('Item1', focus)
            }
        }
    }

    FocusScope {
        id: fs2
        y: 33
        width: 100
        height: 33
        focus: true
        onFocusChanged: {
            console.log('FocusScope2', focus)
        }

        Item {
            focus: true
            onFocusChanged: {
                console.log('Item2', focus)
            }
        }
    }

    FocusScope {
        id: fs3
        y: 66
        width: 100
        height: 34
        focus: true
        onFocusChanged: {
            console.log('FocusScope3', focus)
        }

        Item {
            focus: true
            onFocusChanged: {
                console.log('Item3', focus)
            }
        }
    }

    Component.onCompleted: {
        Qt.quit()
    }
}
