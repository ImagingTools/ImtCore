import QtQuick

Item {
    id: root
    width: 100
    height: 100

    Item {
        Component.onCompleted: {
            console.log('onCompleted 2')
        }

        Item {
            Component.onCompleted: {
                console.log('onCompleted 3')
            }
        }

        Item {
            Component.onCompleted: {
                console.log('onCompleted 22')
            }
        }
    }

    Item {
        Component.onCompleted: {
            console.log('onCompleted 11')
        }
    }

    Item {
        Component.onCompleted: {
            console.log('onCompleted 111')
        }
    }

    Component.onCompleted: {
        console.log('onCompleted 1')
        Qt.quit()
    }
}
