import QtQuick

Item {
   width: row.width
   height: row.height

   onWidthChanged: {
       console.log('Item.onWidthChanged', width)
   }
   onHeightChanged: {
       console.log('Item.onHeightChanged', height)
   }

   Component.onCompleted: {
        console.log("Item.onCompleted")
        Qt.quit()
    }

   Row {
        id: row
        spacing: 10
        onWidthChanged: {
            console.log('Row.onWidthChanged', width)
        }
        onHeightChanged: {
            console.log('Row.onHeightChanged', height)
        }

        Component.onCompleted: {
            console.log("Row.onCompleted")
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
