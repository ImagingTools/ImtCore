import QtQuick

Item {
    width: 100
    height: 100
    Item {
        width: parent.width
        height: parent.height
        property int sizeH: height
        property int sizeW: width
        onWidthChanged: {
            console.log('onWidthChanged 1')
        }
        onHeightChanged: {
            console.log('onHeightChanged 1')
        }
        onSizeWChanged: {
            console.log('onSizeWChanged 1')
        }
        onSizeHChanged: {
            console.log('onSizeHChanged 1')
        }

        Item {
            width: parent.width
            height: parent.height
            property int sizeH: height
            property int sizeW: width
            onWidthChanged: {
                console.log('onWidthChanged 2')
            }
            onHeightChanged: {
                console.log('onHeightChanged 2')
            }
            onSizeWChanged: {
                console.log('onSizeWChanged 2')
            }
            onSizeHChanged: {
                console.log('onSizeHChanged 2')
            }
            Item {
                width: parent.width
                height: parent.height
                property int sizeH: height
                property int sizeW: width
                onWidthChanged: {
                    console.log('onWidthChanged 3')
                }
                onHeightChanged: {
                    console.log('onHeightChanged 3')
                }
                onSizeWChanged: {
                    console.log('onSizeWChanged 3')
                }
                onSizeHChanged: {
                    console.log('onSizeHChanged 3')
                }
            }
        }
    }
}