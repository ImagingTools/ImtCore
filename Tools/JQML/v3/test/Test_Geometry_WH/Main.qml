import QtQuick

Item {
    width: 100
    height: 100
    Item {
        width: parent.width
        height: parent.height
        onWidthChanged: {
            console.log('onWidthChanged 1')
        }
        onHeightChanged: {
            console.log('onHeightChanged 1')
        }
        Item {
            width: parent.width
            height: parent.height
            onWidthChanged: {
                console.log('onWidthChanged 2')
            }
            onHeightChanged: {
                console.log('onHeightChanged 2')
            }
            Item {
                width: parent.width
                height: parent.height
                onWidthChanged: {
                    console.log('onWidthChanged 3')
                }
                onHeightChanged: {
                    console.log('onHeightChanged 3')
                }
            }
        }
    }
}