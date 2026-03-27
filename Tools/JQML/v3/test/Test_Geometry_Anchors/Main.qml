import QtQuick

Item {
    width: 100
    height: 100
    Item {
        anchors.fill: parent
        onWidthChanged: {
            console.log('onWidthChanged 1')
        }
        onHeightChanged: {
            console.log('onHeightChanged 1')
        }
        Item {
            anchors.fill: parent
            onWidthChanged: {
                console.log('onWidthChanged 2')
            }
            onHeightChanged: {
                console.log('onHeightChanged 2')
            }
            Item {
                anchors.fill: parent
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