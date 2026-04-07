import QtQuick

Item {
   width: 500
   height: 500

   Component.onCompleted: {
        console.log("Item.onCompleted")
    }

   Column {
        spacing: 10
        onWidthChanged: {
            console.log('onWidthChanged', width)
        }
        onHeightChanged: {
            console.log('onHeightChanged', height)
        }

        Component.onCompleted: {
            console.log("Column.onCompleted")
        }

        Item {
            width: 50
            height: 50
        }

        Item {
            width: 35
            height: 35
        }

        Item {
            width: 75
            height: 75
        }
   }
}